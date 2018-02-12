// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "SoundListener.h"

namespace Jwl
{
	Entity::WeakPtr SoundListener::listener;

	SoundListener::SoundListener(Entity& _owner)
		: Component(_owner)
	{
		if (GetListener())
		{
			// If there already is an active SoundListener, we don't disturb it.
			_owner.Disable<SoundListener>();
		}
		else
		{
			listener = owner.GetWeakPtr();
		}
	}

	Entity::Ptr SoundListener::GetListener()
	{
		return listener.lock();
	}

	void SoundListener::OnDisable()
	{
		auto listenerEntity = GetListener();

		// If we are in fact the currently active listener, we can be disabled.
		if (listenerEntity.get() == &owner)
		{
			listener.reset();
		}
	}
	
	void SoundListener::OnEnable()
	{
		// If there is already an active listener, we must disable it
		// Because OpenAL only supports one active listener at a time.
		if (auto listenerEntity = GetListener())
		{
			listenerEntity->Disable<SoundListener>();
		}

		listener = owner.GetWeakPtr();
	}
}
