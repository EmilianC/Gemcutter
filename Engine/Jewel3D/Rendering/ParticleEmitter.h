// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Resource/ParticleBuffer.h"
#include "Jewel3D/Resource/ParticleFunctor.h"
#include "Jewel3D/Resource/UniformBuffer.h"
#include "Jewel3D/Utilities/Random.h"

namespace Jwl
{
	//- Spawns particles and updates their positions.
	//- Can be extended with ParticleFunctors to add custom functionality.
	class ParticleEmitter : public Component<ParticleEmitter>
	{
		REFLECT_PRIVATE;
	public:
		ParticleEmitter(Entity& owner, unsigned maxParticles = 100);
		ParticleEmitter& operator=(const ParticleEmitter&);

		enum Type : unsigned
		{
			Omni,
			Box
		};

		void Warmup(float time);
		void Update();

		unsigned GetNumAliveParticles() const;
		unsigned GetNumMaxParticles() const;
		unsigned GetVAO() const;

		//- Sets the Size behaviour of particles if no functors manipulate size.
		void SetSizeStartEnd(const vec2& start, const vec2& end);
		void SetSizeStartEnd(const vec2& constant);
		//- Sets the color behaviour of particles if no functors manipulate color.
		void SetColorStartEnd(const vec3& start, const vec3& end);
		void SetColorStartEnd(const vec3& constant);
		//- Sets the alpha behaviour of particles if no functors manipulate alpha.
		void SetAlphaStartEnd(float start, float end);
		void SetAlphaStartEnd(float constant);

		//- Sets the simulation space of the particle system.
		//- When local, the particles will translate and move with the entity.
		//- Particles will be transformed into the new space to avoid a visual pop.
		void SetLocalSpace(bool isLocal);
		bool IsLocalSpace() const;

		UniformBuffer& GetBuffer();

		FunctorList functors;

		RandomRange velocity{ 1.0f, 0.5f };
		RandomRange lifetime{ 8.0f, 1.0f };
		Type spawnType = Omni;
		float spawnPerSecond = 10.0f;

		//- Used for box spawning.
		RandomRange axisX{ 0.0f, 5.0f };
		RandomRange axisY{ 0.0f, 5.0f };
		RandomRange axisZ{ 0.0f, 5.0f };

		//- Used for Omni-directional spawning.
		RandomRange radius{ 0.0f, 5.0f };

		//- When true, the emitter will cease to update, but will still be rendered.
		bool isPaused = false;

	private:
		void UpdateInternal(float deltaTime);

		ParticleBuffer data;
		
		float numToSpawn		= 0.0f;
		bool requiresAgeRatio	= false;
		bool localSpace			= false;
		unsigned maxParticles	= 0;
		unsigned numCurrentParticles = 0;

		UniformBuffer particleParameters;
	};
}

REFLECT(Jwl::ParticleEmitter::Type)<>,
	VALUES <
		REF_VALUE(Omni),
		REF_VALUE(Box)
	>
REF_END;

REFLECT(Jwl::ParticleEmitter) < Component >,
	MEMBERS <
		REF_MEMBER(functors)<>,
		REF_MEMBER(velocity)<>,
		REF_MEMBER(lifetime)<>,
		REF_MEMBER(spawnType)<>,
		REF_MEMBER(spawnPerSecond)<>,
		REF_MEMBER(axisX)<>,
		REF_MEMBER(axisY)<>,
		REF_MEMBER(axisZ)<>,
		REF_MEMBER(radius)<>,
		REF_MEMBER(isPaused)<>,
		REF_MEMBER(localSpace)< Setter<Jwl::ParticleEmitter, bool, &Jwl::ParticleEmitter::SetLocalSpace> >,
		REF_MEMBER(maxParticles)<>,
		REF_MEMBER(numCurrentParticles)< NoSerialize, ReadOnly >
	>
REF_END;
