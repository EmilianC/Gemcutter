// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"

namespace Jwl
{
	// The Entity with this component is also the location where sounds are heard from.
	// The first instance of the component is active be default, others are inactive by default.
	// Setting a SoundListener as active will disable any other instances.
	class SoundListener : public Component<SoundListener>
	{
	public:
		SoundListener(Entity& owner);

		// Gets the currently active SoundListener. There can only ever be one.
		static Entity::Ptr GetListener();

	private:
		// Disabling this component causes audio to stop being heard until a SoundListener becomes active again.
		void OnDisable() final override;
		void OnEnable() final override;

		// A pointer to the active listener. Only one is allowed.
		static Entity::WeakPtr listener;
	};
}
