// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Entity/Entity.h"

namespace gem
{
	// The Entity with this component marks the location at which sounds are heard from.
	// Only one SoundListener can be enabled at a given point in time.
	// Creating or enabling a SoundListener will disable the currently active one.
	// Disabling this component mutes all audio until a SoundListener becomes enabled again.
	class SoundListener : public Component<SoundListener>
	{
	public:
		SoundListener(Entity& owner);

		// Gets the currently active SoundListener. There can only ever be one.
		static Entity::Ptr GetListener();

	private:
		void OnDisable() final override;
		void OnEnable() final override;

		// A pointer to the active listener.
		static Entity::WeakPtr listener;
	};
}
