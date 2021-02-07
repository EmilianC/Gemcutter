// Copyright (c) 2017 Emilian Cioca
#include "ParticleEmitter.h"
#include "gemcutter/Application/Application.h"
#include "gemcutter/Application/Logging.h"

namespace Jwl
{
	ParticleEmitter::ParticleEmitter(Entity& _owner, unsigned _maxParticles)
		: Renderable(_owner)
		, maxParticles(_maxParticles)
	{
		ASSERT(maxParticles > 0, "'maxParticles' must be greater than 0.");

		owner.Tag<ParticleUpdaterTag>();
		InitUniformBuffer();
	}

	ParticleEmitter::ParticleEmitter(Entity& _owner, Material::Ptr material, unsigned _maxParticles)
		: Renderable(_owner, std::move(material))
		, maxParticles(_maxParticles)
	{
		ASSERT(maxParticles > 0, "'maxParticles' must be greater than 0.");

		owner.Tag<ParticleUpdaterTag>();
		InitUniformBuffer();
	}

	ParticleEmitter::~ParticleEmitter()
	{
		owner.RemoveTag<ParticleUpdaterTag>();
	}

	ParticleEmitter& ParticleEmitter::operator=(const ParticleEmitter& other)
	{
		velocity = other.velocity;
		lifetime = other.lifetime;
		spawnType = other.spawnType;
		spawnPerSecond = other.spawnPerSecond;

		axisX = other.axisX;
		axisY = other.axisY;
		axisZ = other.axisZ;
		radius = other.radius;

		data = other.data;
		functors = other.functors;

		numToSpawn = 0.0f;
		localSpace = false;
		maxParticles = other.maxParticles;
		numCurrentParticles = 0;

		particleParameters.Copy(other.particleParameters);

		return *this;
	}

	void ParticleEmitter::Warmup(float time, float step)
	{
		ASSERT(time > 0.0f, "Warmup time must be greater than 0.");
		ASSERT(step > 0.0f, "Warmup step must be greater than 0.");

		while (time > step)
		{
			UpdateInternal(step);

			time -= step;
		}

		// Finish the rest of the time and upload The data to the GPU.
		if (time > 0.0f)
		{
			UpdateInternal(time);
		}

		data.Update(numCurrentParticles);
	}

	void ParticleEmitter::Update()
	{
		if (!isPaused)
		{
			UpdateInternal(Application.GetDeltaTime());
			// Upload data to GPU.
			data.Update(numCurrentParticles);
		}
		else
		{
			bool updateBuffer = false;
			for (auto& functor : functors.GetAll())
			{
				if (functor->UpdateWhenPaused())
				{
					updateBuffer = true;
					functor->Update(data, *this, Application.GetDeltaTime());
				}
			}

			if (updateBuffer)
			{
				data.Update(numCurrentParticles);
			}
		}
	}

	unsigned ParticleEmitter::GetNumAliveParticles() const
	{
		return numCurrentParticles;
	}

	unsigned ParticleEmitter::GetNumMaxParticles() const
	{
		return maxParticles;
	}

	unsigned ParticleEmitter::GetVAO() const
	{
		return data.GetVAO();
	}

	void ParticleEmitter::SetSizeStartEnd(const vec2& start, const vec2& end)
	{
		particleParameters.SetUniform("StartSize", start);
		particleParameters.SetUniform("EndSize", end);
	}

	void ParticleEmitter::SetSizeStartEnd(const vec2& constant)
	{
		SetSizeStartEnd(constant, constant);
	}

	void ParticleEmitter::SetColorStartEnd(const vec3& start, const vec3& end)
	{
		particleParameters.SetUniform("StartColor", start);
		particleParameters.SetUniform("EndColor", end);
	}

	void ParticleEmitter::SetColorStartEnd(const vec3& constant)
	{
		SetColorStartEnd(constant, constant);
	}

	void ParticleEmitter::SetAlphaStartEnd(float start, float end)
	{
		particleParameters.SetUniform("StartAlpha", start);
		particleParameters.SetUniform("EndAlpha", end);
	}

	void ParticleEmitter::SetAlphaStartEnd(float constant)
	{
		SetAlphaStartEnd(constant, constant);
	}

	void ParticleEmitter::SetLocalSpace(bool isLocal)
	{
		if (localSpace == isLocal)
			return;

		variants.Switch("JWL_PARTICLE_LOCAL_SPACE", isLocal);

		vec3 transform;
		if (isLocal)
		{
			// We are currently in world space and need to switch to local.
			transform = owner.GetWorldTransform().GetFastInverse().GetTranslation();
		}
		else
		{
			// We are currently in local space and need to switch to world.
			transform = owner.GetWorldTransform().GetTranslation();
		}

		for (unsigned i = 0; i < numCurrentParticles; ++i)
		{
			data.positions[i] += transform;
		}

		localSpace = isLocal;
	}

	bool ParticleEmitter::IsLocalSpace() const
	{
		return localSpace;
	}

	const UniformBuffer& ParticleEmitter::GetBuffer() const
	{
		return particleParameters;
	}

	UniformBuffer& ParticleEmitter::GetBuffer()
	{
		return particleParameters;
	}

	void ParticleEmitter::UpdateInternal(float deltaTime)
	{
		ASSERT(maxParticles != 0, "Expected max particle count to be greater than 0.");
		ASSERT(spawnPerSecond >= 0.0f, "'spawnPerSecond' cannot be a negative value.");

		if (functors.dirty)
		{
			EnumFlags<ParticleBuffers> requirements = ParticleBuffers::None;
			for (auto& functor : functors.GetAll())
			{
				requirements |= functor->GetRequirements();
			}

			requiresAgeRatio =
				!requirements.Has(ParticleBuffers::Size) ||
				!requirements.Has(ParticleBuffers::Color) ||
				!requirements.Has(ParticleBuffers::Alpha);

			/* Update shader variant to match the buffers and effect requirements */
			variants.Switch("JWL_PARTICLE_SIZE", requirements.Has(ParticleBuffers::Size));
			variants.Switch("JWL_PARTICLE_COLOR", requirements.Has(ParticleBuffers::Color));
			variants.Switch("JWL_PARTICLE_ALPHA", requirements.Has(ParticleBuffers::Alpha));
			variants.Switch("JWL_PARTICLE_ROTATION", requirements.Has(ParticleBuffers::Rotation));
			variants.Switch("JWL_PARTICLE_AGERATIO", requiresAgeRatio);

			if (requiresAgeRatio)
			{
				// We are using uniform [start, end] values for some properties and require the age of the particle
				// as a percentage. This will LERP in the shader based on the global [start, end] values.
				requirements |= ParticleBuffers::AgeRatio;
			}

			data.SetBuffers(maxParticles, requirements);
			functors.dirty = false;
		}

		/* Update existing particles */
		for (unsigned i = 0; i < numCurrentParticles;)
		{
			data.ages[i] += deltaTime;
			if (!data.IsAlive(i))
			{
				// Remove the particle by replacing it with the one at the top of the stack.
				data.Kill(i, --numCurrentParticles);
				continue;
			}

			data.positions[i] += data.velocities[i] * deltaTime;
			++i;
		}

		/* Run Update Functors */
		for (auto& functor : functors.GetAll())
		{
			functor->Update(data, *this, deltaTime);
		}

		/* Create new particles */
		numToSpawn += spawnPerSecond * deltaTime;
		unsigned initialCount = numCurrentParticles;

		while (
			// We have not reached the particle cap and...
			numCurrentParticles < maxParticles &&
			// We have more particles to generate this frame...
			numToSpawn >= 1.0f)
		{
			if (spawnType == Omni)
			{
				data.positions[numCurrentParticles] = RandomDirection() * radius.Random();
			}
			else
			{
				data.positions[numCurrentParticles] = vec3(axisX.Random(), axisY.Random(), axisZ.Random());
			}

			// Distribute lifetime between frames.
			data.ages[numCurrentParticles] = RandomRange(0.0f, deltaTime);
			data.lifetimes[numCurrentParticles] = lifetime.Random();

			// Send the particle in a random direction, with a velocity between our range.
			data.velocities[numCurrentParticles] = RandomDirection() * velocity.Random();

			numCurrentParticles++;
			numToSpawn -= 1.0f;
		}

		// Transform new particles into the correct space.
		if (!localSpace)
		{
			vec3 transform = owner.GetWorldTransform().GetTranslation();

			for (unsigned i = initialCount; i < numCurrentParticles; ++i)
			{
				data.positions[i] += transform;
			}
		}

		// Particle Functors get a chance to initialize new particles here.
		for (auto& functor : functors.GetAll())
		{
			functor->Init(data, *this, initialCount, numCurrentParticles - initialCount);
		}

		// Percentage of lifespan calculated here.
		if (requiresAgeRatio)
		{
			for (unsigned i = 0; i < numCurrentParticles; ++i)
			{
				data.ageRatios[i] = data.ages[i] / data.lifetimes[i];
			}
		}
	}

	void ParticleEmitter::InitUniformBuffer()
	{
		particleParameters.AddUniform<vec2>("StartSize");
		particleParameters.AddUniform<vec2>("EndSize");
		particleParameters.AddUniform<vec3>("StartColor");
		particleParameters.AddUniform<vec3>("EndColor");
		particleParameters.AddUniform<float>("StartAlpha");
		particleParameters.AddUniform<float>("EndAlpha");
		particleParameters.InitBuffer();

		particleParameters.SetUniform("StartSize", vec2(1.0f));
		particleParameters.SetUniform("EndSize", vec2(0.5f));
		particleParameters.SetUniform("StartColor", vec3(1.0f));
		particleParameters.SetUniform("EndColor", vec3(1.0f));
		particleParameters.SetUniform("StartAlpha", 1.0f);
		particleParameters.SetUniform("EndAlpha", 0.0f);
	}
}
