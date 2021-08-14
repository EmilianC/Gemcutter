// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Resource/VertexArray.h"
#include "gemcutter/Utilities/EnumFlags.h"

namespace gem
{
	struct vec2;
	struct vec3;

	enum class ParticleAttributes : unsigned
	{
		None = 0,
		Size = 1,      // vec2
		Color = 2,     // vec3
		Alpha = 4,     // float
		Rotation = 8,  // float
		AgeRatio = 16  // float (age / lifetime)
	};

	class ParticleBuffer
	{
	public:
		ParticleBuffer(unsigned maxParticles);
		~ParticleBuffer();

		void Unload();

		void SetAttributes(EnumFlags<ParticleAttributes> attributes);
		// Uploads data to the GPU buffers.
		void Update(unsigned activeParticles);

		void Kill(unsigned index, unsigned last);
		bool IsAlive(unsigned index) const;

		EnumFlags<ParticleAttributes> GetAttributes() const;
		VertexArray::Ptr GetArray() const;
		unsigned GetMaxParticles() const;

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
		unsigned TotalBufferSize() const;

		EnumFlags<ParticleAttributes> attributes = ParticleAttributes::None;
		unsigned maxParticles = 0;

		VertexArray::Ptr array;
		VertexBuffer::Ptr buffer;
	};
}
