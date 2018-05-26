// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Jewel3D/Application/Logging.h"
#include "Event.h"

namespace Jwl
{
	EventQueueSingleton EventQueue;

	void EventQueueSingleton::Push(std::unique_ptr<EventBase> e)
	{
#ifdef _DEBUG
		ASSERT(!inDispatch,
			"It is dangerous to add a new event to the queue while currently inside a global Dispatch().\n"
			"Consider using Dispatch(const EventBase&) instead.");
#endif

		if (e->HasListeners())
		{
			eventQueue.push(std::move(e));
		}
	}

	void EventQueueSingleton::Dispatch(const EventBase& e) const
	{
		e.Raise();
	}

	void EventQueueSingleton::Dispatch()
	{
		// Events should not be posted during this call.
#ifdef _DEBUG
		inDispatch = true;
#endif

		// Sequence through all events.
		while (!eventQueue.empty())
		{
			eventQueue.front()->Raise();
			eventQueue.pop();
		}

		// Events can be posted again.
#ifdef _DEBUG
		inDispatch = false;
#endif
	}
}
