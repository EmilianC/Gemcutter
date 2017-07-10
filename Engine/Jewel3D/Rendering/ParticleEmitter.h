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
	public:
		ParticleEmitter(Entity& owner, u32 maxParticles = 100);
		ParticleEmitter& operator=(const ParticleEmitter&);

		enum Type : u32
		{
			Omni,
			Box
		};

		void Warmup(f32 time);
		void Update();

		u32 GetNumAliveParticles() const;
		u32 GetNumMaxParticles() const;
		u32 GetVAO() const;

		//- Sets the Size behaviour of particles if no functors manipulate size.
		void SetSizeStartEnd(const vec2& start, const vec2& end);
		void SetSizeStartEnd(const vec2& constant);
		//- Sets the color behaviour of particles if no functors manipulate color.
		void SetColorStartEnd(const vec3& start, const vec3& end);
		void SetColorStartEnd(const vec3& constant);
		//- Sets the alpha behaviour of particles if no functors manipulate alpha.
		void SetAlphaStartEnd(f32 start, f32 end);
		void SetAlphaStartEnd(f32 constant);

		//- Sets the simulation space of the particle system.
		//- When local, the particles will translate and move with the entity.
		//- Particles will be transformed into the new space to avoid a visual pop.
		void SetLocalSpace(bool isLocal);
		bool IsLocalSpace() const;

		UniformBuffer& GetBuffer();

		FunctorList functors;

		Range velocity{ 0.5f, 1.0f };
		Range lifetime{ 5.0f, 10.0f };
		Type spawnType = Omni;
		f32 spawnPerSecond = 10.0f;

		//- Used for box spawning.
		Range axisX{ -1.0f, 1.0f };
		Range axisY{ -1.0f, 1.0f };
		Range axisZ{ -1.0f, 1.0f };

		//- Used for Omni-directional spawning.
		Range radius{ 0.0f, 1.0f };

		//- When true, the emitter will cease to update, but will still be rendered.
		bool isPaused = false;

	private:
		void UpdateInternal(f32 deltaTime);

		ParticleBuffer data;
		
		f32 numToSpawn		= 0.0f;
		bool requiresAgeRatio	= false;
		bool localSpace			= false;
		u32 maxParticles	= 0;
		u32 numCurrentParticles = 0;

		UniformBuffer particleParameters;
	};
}
