// Copyright (c) 2017 Emilian Cioca
#include "Event.h"

#ifdef _DEBUG
#include "gemcutter/Application/Logging.h"
#endif

namespace gem
{
	EventQueueSingleton EventQueue;

	void EventQueueSingleton::Push(std::unique_ptr<EventBase> e)
	{
		eventQueue.push(std::move(e));
	}

	void EventQueueSingleton::Dispatch(const EventBase& e) const
	{
		e.Raise();
	}

	void EventQueueSingleton::Dispatch()
	{
		ASSERT(!dispatching,
			"Call to Dispatch() cannot be executed because the event queue is already being dispatched higher in the call stack.\n"
			"Consider using Dispatch(const EventBase&) if an event must be processed here immediately.");

		dispatching = true;
		while (!eventQueue.empty())
		{
			eventQueue.front()->Raise();
			eventQueue.pop();
		}
		dispatching = false;
	}

	bool EventQueueSingleton::IsDispatching() const
	{
		return dispatching;
	}
}
