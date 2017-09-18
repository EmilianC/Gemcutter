// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "ParticleEmitter.h"
#include "Material.h"
#include "Jewel3D/Application/Application.h"
#include "Jewel3D/Application/Logging.h"

namespace Jwl
{
	ParticleEmitter::ParticleEmitter(Entity& _owner, unsigned _maxParticles)
		: Component(_owner)
		, maxParticles(_maxParticles)
	{
		ASSERT(maxParticles > 0, "'maxParticles' must be greater than 0.");

		particleParameters.AddUniform("StartSize", sizeof(vec2));
		particleParameters.AddUniform("EndSize", sizeof(vec2));
		particleParameters.AddUniform("StartColor", sizeof(vec3));
		particleParameters.AddUniform("EndColor", sizeof(vec3));
		particleParameters.AddUniform("StartAlpha", sizeof(float));
		particleParameters.AddUniform("EndAlpha", sizeof(float));
		particleParameters.InitBuffer();

		particleParameters.SetUniform("StartSize", vec2(1.0f));
		particleParameters.SetUniform("EndSize", vec2(0.5f));
		particleParameters.SetUniform("StartColor", vec3(1.0f));
		particleParameters.SetUniform("EndColor", vec3(1.0f));
		particleParameters.SetUniform("StartAlpha", 1.0f);
		particleParameters.SetUniform("EndAlpha", 0.0f);
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
		if (isPaused)
			return;

		UpdateInternal(Application.GetDeltaTime());

		// Upload data to GPU.
		data.Update(numCurrentParticles);
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

		owner.Get<Material>().variantDefinitions.Switch("JWL_PARTICLE_LOCAL_SPACE", isLocal);

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
			for (const auto& functor : functors.GetAll())
			{
				requirements |= functor->GetRequirements();
			}

			/* Update shader variant to match the buffers and effect requirements */
			auto& material = owner.Get<Material>();

			requiresAgeRatio = false;
			if (requirements.Has(ParticleBuffers::Size))
			{
				material.variantDefinitions.Define("JWL_PARTICLE_SIZE");
			}
			else
			{
				material.variantDefinitions.Undefine("JWL_PARTICLE_SIZE");
				requiresAgeRatio = true;
			}

			if (requirements.Has(ParticleBuffers::Color))
			{
				material.variantDefinitions.Define("JWL_PARTICLE_COLOR");
			}
			else
			{
				material.variantDefinitions.Undefine("JWL_PARTICLE_COLOR");
				requiresAgeRatio = true;
			}

			if (requirements.Has(ParticleBuffers::Alpha))
			{
				material.variantDefinitions.Define("JWL_PARTICLE_ALPHA");
			}
			else
			{
				material.variantDefinitions.Undefine("JWL_PARTICLE_ALPHA");
				requiresAgeRatio = true;
			}

			if (requirements.Has(ParticleBuffers::Rotation))
			{
				material.variantDefinitions.Define("JWL_PARTICLE_ROTATION");
			}
			else
			{
				material.variantDefinitions.Undefine("JWL_PARTICLE_ROTATION");
				requiresAgeRatio = true;
			}

			if (requiresAgeRatio)
			{
				requirements |= ParticleBuffers::AgeRatio;
				material.variantDefinitions.Define("JWL_PARTICLE_AGERATIO");
			}
			else
			{
				material.variantDefinitions.Undefine("JWL_PARTICLE_AGERATIO");
			}

			data.SetBuffers(maxParticles, requirements);
			functors.dirty = false;
		}

		/* Update existing particles */
		for (unsigned i = 0; i < numCurrentParticles; ++i)
		{
			data.ages[i] += deltaTime;

			if (!data.IsAlive(i))
			{
				// Remove the particle by replacing it with the one at the top of the stack.
				data.Kill(i, --numCurrentParticles);
				continue;
			}

			data.positions[i] += data.velocities[i] * deltaTime;
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
}
