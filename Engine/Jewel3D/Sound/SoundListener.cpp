// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "SoundListener.h"
#include "Jewel3D/Application/Logging.h"

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

		// if we are in fact the currently active listener, we can be disabled.
		if (listenerEntity.get() == &owner)
		{
			listener.reset();
		}
	}
	
	void SoundListener::OnEnable()
	{
		// if there is another listener we must disable it first.
		// OpenAL only supports one listener at a time.
		if (auto listenerEntity = GetListener())
		{
			listenerEntity->Disable<SoundListener>();
		}

		listener = owner.GetWeakPtr();
	}
}
