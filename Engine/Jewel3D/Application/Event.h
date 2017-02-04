// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Utilities/Singleton.h"

#include <vector>
#include <memory>
#include <functional>

namespace Jwl
{
	class EventBase;

	//- The base class for Listener objects. Used internally.
	class ListenerBase
	{
	public:
		virtual ~ListenerBase() = default;

		virtual void Notify(const EventBase&) = 0;
	};

	//- The base class for event objects. Used internally.
	//- Carries with it data that is relevant to that event.
	class EventBase
	{
	public:
		virtual ~EventBase() = default;
		//- Returns a vector of all objects currently listening for the derived class event.
		virtual const std::vector<ListenerBase*>& GetListeners() const = 0;
		//- Returns true if at least one EventListener responds to this event.
		virtual bool HasListeners() const = 0;
		//- Notifies all listeners of the derived class event by invoking their callback functions.
		virtual void DistributeCallbacks() const = 0;
	};

	//- The main event object. You should inherit from this to create your own custom events.
	//- The template parameter must be the derived class. For example:
	//	class PlayerDeath : public Event<PlayerDeath> {};
	template <class derived>
	class Event : public EventBase
	{
	public:
		virtual ~Event() = default;
		//- Returns a vector of all Listeners currently subscribed to the derived class event.
		virtual const std::vector<ListenerBase*>& GetListeners() const final override
		{
			return GetListenersStatic();
		}

		//- Returns true if at least one Listener responds to the derived class event.
		virtual bool HasListeners() const final override
		{
			return HasListenersStatic();
		}

		//- Notifies all Listeners of this event by invoking their callback functions.
		virtual void DistributeCallbacks() const final override
		{
			for (unsigned i = 0; i < listeners.size(); i++)
			{
				listeners[i]->Notify(*this);
			}
		}

		//- Returns a vector of all objects currently listening for the derived class event.
		static const std::vector<ListenerBase*>& GetListenersStatic()
		{
			return listeners;
		}

		//- Returns true if at least one Listener responds to the derived class event.
		static bool HasListenersStatic()
		{
			return !listeners.empty();
		}

		//- Adds subscribes a Listener to receive callbacks from this type of event.
		static void Subscribe(ListenerBase& listener)
		{
			ASSERT(!IsListener(listener), "The Listener is already listening to the specified event.");

			listeners.push_back(&listener);
		}

		//- Stops a Listener from receiving callbacks from this type of event.
		static void Unsubscribe(ListenerBase& listener)
		{
			ASSERT(IsListener(listener), "The Listener is not listening to the specified event.");

			listeners.erase(std::find(listeners.begin(), listeners.end(), &listener));
		}

		//- Returns true if the given Listener responds to this event.
		static bool IsListener(ListenerBase& listener)
		{
			return std::find(listeners.begin(), listeners.end(), &listener) != listeners.end();
		}

	private:
		//- All Listeners of the derived class event.
		static std::vector<ListenerBase*> listeners;
	};
	
	//- This class gives you the ability to respond to events. All events are distributed to these listeners
	//	which then invoke your callback function.
	//- The template parameter must be the event object that the listener will listen for.
	//	For example: Listener<PlayerDeath> OnPlayerDeath(&myFunc);
	template <class EventObj>
	class Listener : public ListenerBase
	{
		static_assert(std::is_base_of<EventBase, EventObj>::value, "Template argument must inherit from Event.");
	public:
		using EventType = EventObj;

		//- Constructs the listener without a connection to a callback function.
		Listener()
		{
			EventObj::Subscribe(*this);
		}

		//- Constructs the listener with a callback function.
		Listener(std::function<void(const EventObj&)> callbackFunc)
			: callback(callbackFunc)
		{
			EventObj::Subscribe(*this);
		}

		//- The destructor unsubscribes from the event for you.
		~Listener()
		{
			EventObj::Unsubscribe(*this);
		}

		//- Invokes the callback function while forwarding the given event.
		//- The event object must be an instance of the template parameter, EventObj.
		virtual void Notify(const EventBase& e) final override
		{
			ASSERT(typeid(e) == typeid(EventObj), "Event callback received event object of the wrong type.");

			if (callback)
			{
				callback(static_cast<const EventObj&>(e));
			}
		}

		//- The callback function called when an event object of the templated type
		//	is distributed by the event manager.
		std::function<void(const EventObj&)> callback;
	};

	//- This singleton class handles the distribution of events across all listeners.
	static class EventQueue : public Singleton<class EventQueue>
	{
	public:
		//- Add a new event to the queue. The event will be distributed to all listeners of its type.
		//-- theEvent: An instance of EventData that stores relevant data for the event.
		void Push(std::unique_ptr<EventBase> theEvent);

		//- Instantly distributes an event across listeners. It is not added to the queue.
		//-- theEvent: An instance of EventData that stores relevant data for the event.
		void Dispatch(const EventBase& theEvent) const;

		//- Sequences through the queue of events and distributes them to all the listeners.
		//- There will be no events in the queue when this function returns.
		//- It is illegal, and unsafe, to post events with PostEvent() while this functions is executing.
		void Dispatch();

	private:
		//- Events waiting to be processed.
		std::vector<std::unique_ptr<EventBase>> eventQueue;

#ifdef _DEBUG
		//- Debug flag to stop events from being posted using Queue() during a call to DispatchQueue().
		bool inDispatchQueue;
#endif
	} &EventQueue = Singleton<class EventQueue>::instanceRef;

	template<class derived> std::vector<ListenerBase*> Event<derived>::listeners;
}
