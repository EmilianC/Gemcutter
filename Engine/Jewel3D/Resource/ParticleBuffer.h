// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Utilities/EnumFlags.h"

namespace Jwl
{
	enum class ParticleBuffers : u32
	{
		None = 0,		// Empty buffer
		Size = 1,		// vec2 size
		Color = 2,		// vec3 color
		Alpha = 4,		// f32 alpha
		Rotation = 8,	// f32 rotation
		AgeRatio = 16	// f32 ageRatio (age / lifetime)
	};

	class ParticleBuffer
	{
	public:
		ParticleBuffer();
		ParticleBuffer(const ParticleBuffer&);
		ParticleBuffer(ParticleBuffer&&);
		ParticleBuffer& operator=(const ParticleBuffer&);
		~ParticleBuffer();

		void SetBuffers(u32 numParticles, EnumFlags<ParticleBuffers> requirements);
		void Unload();
		//- Uploads data to the GPU buffers.
		void Update(u32 numParticles);

		void Kill(u32 index, u32 last);
		bool IsAlive(u32 index);

		EnumFlags<ParticleBuffers> GetBuffers() const;
		u32 GetVAO() const;
		u32 GetNumParticles() const;

		vec3*	positions	= nullptr;
		vec3*	velocities	= nullptr;
		f32*	ages		= nullptr;
		f32*	lifetimes	= nullptr;
		vec2*	sizes		= nullptr;
		vec3*	colors		= nullptr;
		f32*	alphas		= nullptr;
		f32*	rotations	= nullptr;
		f32*	ageRatios	= nullptr;

	private:
		EnumFlags<ParticleBuffers> buffers = ParticleBuffers::None;
		u32 numParticles = 0;
		u32 VAO = 0;
		u32 VBO = 0;
	};
}
