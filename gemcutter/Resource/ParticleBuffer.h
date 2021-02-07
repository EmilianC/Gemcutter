// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Utilities/EnumFlags.h"

namespace gem
{
	struct vec2;
	struct vec3;

	enum class ParticleBuffers : unsigned
	{
		None = 0,       // Empty buffer
		Size = 1,       // vec2 size
		Color = 2,      // vec3 color
		Alpha = 4,      // float alpha
		Rotation = 8,   // float rotation
		AgeRatio = 16   // float ageRatio (age / lifetime)
	};

	class ParticleBuffer
	{
	public:
		ParticleBuffer();
		ParticleBuffer(const ParticleBuffer&);
		ParticleBuffer(ParticleBuffer&&) noexcept;
		ParticleBuffer& operator=(const ParticleBuffer&);
		~ParticleBuffer();

		void SetBuffers(unsigned numParticles, EnumFlags<ParticleBuffers> buffers);
		void Unload();
		// Uploads data to the GPU buffers.
		void Update(unsigned numParticles);

		void Kill(unsigned index, unsigned last);
		bool IsAlive(unsigned index) const;

		EnumFlags<ParticleBuffers> GetBuffers() const;
		unsigned GetVAO() const;
		unsigned GetNumParticles() const;

		vec3*  positions  = nullptr;
		vec3*  velocities = nullptr;
		float* ages       = nullptr;
		float* lifetimes  = nullptr;
		vec2*  sizes      = nullptr;
		vec3*  colors     = nullptr;
		float* alphas     = nullptr;
		float* rotations  = nullptr;
		float* ageRatios  = nullptr;

	private:
		EnumFlags<ParticleBuffers> buffers = ParticleBuffers::None;
		unsigned numParticles = 0;
		unsigned VAO = 0;
		unsigned VBO = 0;
	};
}
