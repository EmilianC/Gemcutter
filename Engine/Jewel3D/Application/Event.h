// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Utilities/Singleton.h"

#include <functional>
#include <memory>
#include <queue>
#include <vector>

namespace Jwl
{
	//- The base class for event objects.
	class EventBase
	{
		friend class EventQueue;
	public:
		virtual ~EventBase() = default;
		//- Returns true if at least one listener responds to this event.
		virtual bool HasListeners() const = 0;

	private:
		//- Notifies all listeners of the derived class event by invoking their callback functions.
		virtual void Raise() const = 0;
	};

	//- Base class for event listeners. 
	//- Invokes the callback function when an instance of the respective event is dispatched.
	//- The template parameter must be the event object that the listener will subscribe to.
	//	For example: Listener<PlayerDeath> OnPlayerDeath(&myFunc);
	template <class EventObj>
	class Listener
	{
		static_assert(std::is_base_of<EventBase, EventObj>::value, "Template argument must inherit from Event.");
	public:
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

		//- The callback function invoked when an event of the templated type is dispatched.
		std::function<void(const EventObj&)> callback;
	};

	//- You can inherit from this class to create your own custom events.
	//- The template parameter must be the derived class. For example:
	//	class PlayerDeath : public Event<PlayerDeath> {};
	template <class derived>
	class Event : public EventBase
	{
		friend Listener<derived>;
	public:
		virtual ~Event() = default;

		//- Returns true if at least one listener responds to this event.
		virtual bool HasListeners() const final override
		{
			return HasListenersStatic();
		}

		//- Returns a vector of all objects currently listening for this type of event.
		static const auto& GetListenersStatic()
		{
			return listeners;
		}

		//- Returns true if at least one listener responds to this type of event.
		static bool HasListenersStatic()
		{
			return !listeners.empty();
		}

	private:
		//- Notifies all listeners of this event by invoking their callback functions.
		virtual void Raise() const final override
		{
			for (u32 i = 0; i < listeners.size(); i++)
			{
				if (listeners[i]->callback)
				{
					listeners[i]->callback(*static_cast<const derived*>(this));
				}
			}
		}

		//- Subscribes a listener to receive callbacks from this type of event.
		static void Subscribe(Listener<derived>& listener)
		{
			listeners.push_back(&listener);
		}

		//- Stops a listener from receiving callbacks from this type of event.
		static void Unsubscribe(Listener<derived>& listener)
		{
			listeners.erase(std::find(listeners.begin(), listeners.end(), &listener));
		}

		//- All Listeners of the derived class event.
		static std::vector<Listener<derived>*> listeners;
	};
	template<class derived> std::vector<Listener<derived>*> Event<derived>::listeners;
	
	//- This singleton class handles queuing and distribution of events.
	static class EventQueue : public Singleton<class EventQueue>
	{
	public:
		//- Add a new event to the queue.
		//- The event will be distributed to all listeners of its type when Dispatch() is called.
		void Push(std::unique_ptr<EventBase> e);

		//- Instantly distributes an event across listeners. It is not added to the queue.
		void Dispatch(const EventBase& e) const;

		//- Sequences through the queue of events and distributes them to all the listeners.
		//- There will be no events in the queue when this function returns.
		//- It is illegal, and unsafe, to post new events while this function is executing.
		void Dispatch();

	private:
		std::queue<std::unique_ptr<EventBase>> eventQueue;

#ifdef _DEBUG
		//- Debug flag to stop events from being queued during a call to Dispatch().
		bool inDispatch = false;
#endif
	} &EventQueue = Singleton<class EventQueue>::instanceRef;
}
