// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Resource/ParticleBuffer.h"
#include "gemcutter/Resource/Shareable.h"
#include "gemcutter/Utilities/Random.h"

#include <vector>

namespace gem
{
	class ParticleEmitter;

	// Base functor class for changing particle behaviour or spawning particles.
	class ParticleFunctor
	{
	public:
		virtual ~ParticleFunctor() = default;
		// Called to initialize newly spawned particles.
		// New particles will be in the specified index range.
		virtual void Init(ParticleBuffer& particles, ParticleEmitter& emitter, unsigned startIndex, unsigned count);
		// Main update call for the functor.
		virtual void Update(ParticleBuffer& particles, ParticleEmitter& emitter, float deltaTime) = 0;
		// Specifies the required data attributes for this functor to update.
		virtual ParticleAttributes GetRequirements() const { return ParticleAttributes::None; }
		// Specifies if the functor should be executed even when the emitter is paused.
		virtual bool UpdateWhenPaused() const { return false; }
	};

	// A list of ParticleFunctors acting on a ParticleEmitter.
	class FunctorList
	{
		friend ParticleEmitter;
	public:
		FunctorList() = default;
		FunctorList(const FunctorList&);
		FunctorList& operator=(const FunctorList&);

		void Add(std::shared_ptr<ParticleFunctor> functor);

		// Removes all functors.
		void Clear();

		const auto& GetAll() const { return functors; }

	private:
		std::vector<std::shared_ptr<ParticleFunctor>> functors;

		bool dirty = true;
	};

	/* Built-in Particle Functors */
	// Rotates particles.
	class RotationFunc : public ParticleFunctor, public Shareable<RotationFunc>
	{
		friend ShareableAlloc;
		RotationFunc() = default;
		RotationFunc(float rotationSpeed, Range initialRotation = Range(0.0f, 360.0f));

	public:
		void Init(ParticleBuffer& particles, ParticleEmitter& emitter, unsigned startIndex, unsigned count) override;
		void Update(ParticleBuffer& particles, ParticleEmitter& emitter, float deltaTime) override;

		ParticleAttributes GetRequirements() const final override;

		float rotationSpeed = 5.0f;
		Range initialRotation{ 0.0f, 360.0f };
	};
}
