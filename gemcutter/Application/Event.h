// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <functional>
#include <memory>
#include <queue>
#include <vector>

namespace Jwl
{
	// The base class for event objects.
	class EventBase
	{
		friend class EventQueueSingleton;
	public:
		virtual ~EventBase() = default;
		// Returns true if at least one listener responds to this event.
		virtual bool HasListeners() const = 0;

	private:
		// Notifies all listeners of the derived class event by invoking their callback functions.
		virtual void Raise() const = 0;
	};

	// Invokes a callback function when an instance of the respective event is dispatched.
	// The template parameter must be the event object that the listener will subscribe to.
	//  For example: Listener<PlayerDeath> OnPlayerDeath(&myFunc);
	template<class EventObj>
	class Listener
	{
		template<class T> friend class Event;
		static_assert(std::is_base_of_v<EventBase, EventObj>, "Template argument must inherit from Event.");
	public:
		using EventFunc = void(const EventObj&);

		// Subscribes to the event.
		Listener();
		Listener(const std::function<EventFunc>& callback);
		Listener(std::function<EventFunc>&& callback);

		// Unsubscribes from the event.
		~Listener();

		Listener& operator=(const std::function<EventFunc>& callback);
		Listener& operator=(std::function<EventFunc>&& callback);

	private:
		std::function<EventFunc> callback;
	};

	// You can inherit from this class to create your own custom events.
	// The template parameter must be the derived class. For example:
	//	class PlayerDeath : public Event<PlayerDeath> {};
	template<class derived>
	class Event : public EventBase
	{
		friend Listener<derived>;
	public:
		virtual ~Event() = default;

		// Returns true if at least one listener responds to this event.
		bool HasListeners() const final override;
		static bool HasListenersStatic();

		// Returns a vector of all objects currently listening for this type of event.
		static const auto& GetListenersStatic() { return listeners; }

	private:
		void Raise() const final override;

		// Subscribes a listener to be notified from this type of event.
		static void Subscribe(Listener<derived>& listener);
		// Stops a listener from being notified callbacks from this type of event.
		static void Unsubscribe(Listener<derived>& listener);

		// All Listeners of the derived class event.
		static std::vector<Listener<derived>*> listeners;
	};

	// This singleton class handles queuing and distribution of events.
	extern class EventQueueSingleton EventQueue;
	class EventQueueSingleton
	{
	public:
		// Add a new event to the queue.
		// The event will be distributed to all listeners of its type when Dispatch() is called.
		void Push(std::unique_ptr<EventBase> e);

		// Instantly distributes an event across listeners. It is not added to the queue.
		void Dispatch(const EventBase& e) const;

		// Sequences through the queue of events and distributes them to all the listeners.
		// There will be no events in the queue when this function returns.
		// It is illegal, and unsafe, to post new events while this function is executing.
		void Dispatch();

	private:
		std::queue<std::unique_ptr<EventBase>> eventQueue;

#ifdef _DEBUG
		// Stops events from being queued during a call to Dispatch().
		bool inDispatch = false;
#endif
	};
}

#include "Event.inl"
