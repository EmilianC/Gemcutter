// Copyright (c) 2017 Emilian Cioca
#include "SoundListener.h"
#include "gemcutter/Sound/SoundSystem.h"

namespace gem
{
	Entity::WeakPtr SoundListener::listener;

	SoundListener::SoundListener(Entity& _owner)
		: Component(_owner)
	{
		if (!IsEnabled())
		{
			// Do not change any existing state.
			return;
		}

		OnEnable();
	}

	Entity::Ptr SoundListener::GetListener()
	{
		return listener.lock();
	}

	void SoundListener::OnDisable()
	{
		Entity::Ptr listenerEntity = GetListener();

		// If we are in fact the currently active listener, we can be disabled.
		if (listenerEntity == owner)
		{
			listener.reset();
			SoundSystem.Mute();
		}
	}

	void SoundListener::OnEnable()
	{
		// If there is already an active listener, we must disable it
		// because we only support one active listener at a time.
		if (Entity::Ptr listenerEntity = GetListener())
		{
			listenerEntity->Disable<SoundListener>();
		}

		listener = owner.GetWeakPtr();
		SoundSystem.Unmute();
	}
}
