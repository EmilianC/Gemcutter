// Copyright (c) 2017 Emilian Cioca
#include "ParticleFunctor.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Rendering/ParticleEmitter.h"

namespace gem
{
	FunctorList::FunctorList(const FunctorList& other)
		: functors(other.functors)
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

	RotationFunc::RotationFunc(float _rotationSpeed, Range _initialRotation)
		: rotationSpeed(_rotationSpeed)
		, initialRotation(_initialRotation)
	{
	}

	void RotationFunc::Init(ParticleBuffer& particles, ParticleEmitter& emitter, unsigned startIndex, unsigned count)
	{
		ASSERT(startIndex + count <= particles.GetMaxParticles(), "Indices out of range.");

		for (unsigned i = startIndex, end = startIndex + count; i < end; ++i)
		{
			particles.rotations[i] = initialRotation.Random();
		}
	}

	void RotationFunc::Update(ParticleBuffer& particles, ParticleEmitter& emitter, float deltaTime)
	{
		float rotation = rotationSpeed * deltaTime;
		for (unsigned i = 0; i < emitter.GetNumAliveParticles(); ++i)
		{
			particles.rotations[i] += rotation;
		}
	}

	ParticleAttributes RotationFunc::GetRequirements() const
	{
		return ParticleAttributes::Rotation;
	}
}
