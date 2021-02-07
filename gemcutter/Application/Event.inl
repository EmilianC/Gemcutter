// Copyright (c) 2017 Emilian Cioca
namespace gem
{
	template<class derived> std::vector<Listener<derived>*> Event<derived>::listeners;

	template<class EventObj>
	Listener<EventObj>::Listener()
	{
		EventObj::Subscribe(*this);
	}

	template<class EventObj>
	Listener<EventObj>::Listener(const std::function<EventFunc>& _callback)
		: callback(_callback)
	{
		EventObj::Subscribe(*this);
	}

	template<class EventObj>
	Listener<EventObj>::Listener(std::function<EventFunc>&& _callback)
		: callback(std::move(_callback))
	{
		EventObj::Subscribe(*this);
	}

	template<class EventObj>
	Listener<EventObj>::~Listener()
	{
		EventObj::Unsubscribe(*this);
	}

	template<class EventObj>
	Listener<EventObj>& Listener<EventObj>::operator=(const std::function<EventFunc>& _callback)
	{
		callback = _callback;
		return *this;
	}

	template<class EventObj>
	Listener<EventObj>& Listener<EventObj>::operator=(std::function<EventFunc>&& _callback)
	{
		callback = std::move(_callback);
		return *this;
	}

	template<class derived>
	bool Event<derived>::HasListeners() const
	{
		return HasListenersStatic();
	}

	template<class derived>
	bool Event<derived>::HasListenersStatic()
	{
		return !listeners.empty();
	}

	template<class derived>
	void Event<derived>::Raise() const
	{
		for (unsigned i = 0; i < listeners.size(); ++i)
		{
			if (listeners[i]->callback)
			{
				listeners[i]->callback(*static_cast<const derived*>(this));
			}
		}
	}

	template<class derived>
	void Event<derived>::Subscribe(Listener<derived>& listener)
	{
		listeners.push_back(&listener);
	}

	template<class derived>
	void Event<derived>::Unsubscribe(Listener<derived>& listener)
	{
		listeners.erase(std::find(listeners.begin(), listeners.end(), &listener));
	}
}
