// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "ParticleFunctor.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Rendering/ParticleEmitter.h"

namespace Jwl
{
	FunctorList::FunctorList(const FunctorList& other)
		: functors(other.functors)
		, dirty(true)
	{
	}

	FunctorList& FunctorList::operator=(const FunctorList& other)
	{
		functors = other.functors;
		dirty = true;

		return *this;
	}

	void FunctorList::Clear()
	{
		functors.clear();
	}

	RotationFunc::RotationFunc(f32 _rotationSpeed, Range _initialRotation)
		: rotationSpeed(_rotationSpeed)
		, initialRotation(_initialRotation)
	{
	}

	void RotationFunc::Init(ParticleBuffer& particles, ParticleEmitter& emitter, u32 startIndex, u32 count)
	{
		ASSERT(startIndex + count <= particles.GetNumParticles(), "Indices out of range.");

		for (u32 i = startIndex, end = startIndex + count; i < end; i++)
		{
			particles.rotations[i] = initialRotation.Random();
		}
	}

	void RotationFunc::Update(ParticleBuffer& particles, ParticleEmitter& emitter, f32 deltaTime)
	{
		f32 rotation = rotationSpeed * deltaTime;
		for (u32 i = 0; i < emitter.GetNumAliveParticles(); i++)
		{
			particles.rotations[i] += rotation;
		}
	}

	ParticleBuffers RotationFunc::GetRequirements() const
	{
		return ParticleBuffers::Rotation;
	}
}