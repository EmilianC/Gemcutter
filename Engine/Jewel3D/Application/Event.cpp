// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Event.h"

namespace Jwl
{
	void EventQueue::Push(std::unique_ptr<EventBase> theEvent)
	{
		#ifdef _DEBUG
			ASSERT(!inDispatchQueue, 
				"It is dangerous to add a new event to the queue while currently inside a global Dispatch().\n"
				"Consider using Dispatch(const EventBase&) instead.");
		#endif

		if (theEvent->HasListeners())
		{
			eventQueue.push_back(std::move(theEvent));
		}
	}

	void EventQueue::Dispatch(const EventBase& theEvent) const
	{
		theEvent.DistributeCallbacks();
	}

	void EventQueue::Dispatch()
	{
		// Events should not be posted during this call.
		#ifdef _DEBUG
			inDispatchQueue = true;
		#endif

		// Sequence through all events.
		for (unsigned i = 0; i < eventQueue.size(); i++)
		{
			eventQueue[i]->DistributeCallbacks();
		}

		// Events can be posted again.
		#ifdef _DEBUG
			inDispatchQueue = false;
		#endif

		eventQueue.clear();
	}
}
