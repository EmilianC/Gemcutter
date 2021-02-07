// Copyright (c) 2017 Emilian Cioca
#include "Event.h"

#ifdef _DEBUG
#include "gemcutter/Application/Logging.h"
#endif

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

		eventQueue.push(std::move(e));
	}

	void EventQueueSingleton::Dispatch(const EventBase& e) const
	{
		e.Raise();
	}

	void EventQueueSingleton::Dispatch()
	{
#ifdef _DEBUG
		// Events should not be posted during this call.
		inDispatch = true;
#endif

		while (!eventQueue.empty())
		{
			eventQueue.front()->Raise();
			eventQueue.pop();
		}

#ifdef _DEBUG
		// Events can be posted again.
		inDispatch = false;
#endif
	}
}
