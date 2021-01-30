// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Application/Event.h"
#include "Jewel3D/Entity/Entity.h"

#include <functional>

namespace Jwl
{
	// Responds to events given from an EventDispatcher component higher in the hierarchy.
	template<class EventObj>
	class ProxyListener : public Component<ProxyListener<EventObj>>
	{
		static_assert(std::is_base_of_v<EventBase, EventObj>, "Template argument must inherit from Event.");
	public:
		ProxyListener(Entity& owner)
			: Component(owner)
		{
		}

		// Your function should return 'true' if the event is handled,
		// This will cause it to stop propagating down the hierarchy.
		std::function<bool(const EventObj&)> callback;
	};

	// Propagates an event through a hierarchy of Entities.
	template<class EventObj>
	class EventDispatcher : public Component<EventDispatcher<EventObj>>
	{
		static_assert(std::is_base_of_v<EventBase, EventObj>, "Template argument must inherit from Event.");
	public:
		EventDispatcher(Entity& owner)
			: Component(owner)
		{
			OnEnable();
		}

		void OnEnable() final override
		{
			listener.callback = [owner](auto& e) { Distribute(owner, e); };
		}

		void OnDisable() final override
		{
			listener.callback = nullptr;
		}

	private:
		// Propagates the event through the hierarchy, breadth first, notifying ProxyListeners.
		// Once a listener has handled the event by returning true from it's callback, propagation stops.
		static bool Distribute(Entity& ent, const EventObj& e)
		{
			auto& children = ent.GetChildren();

			for (unsigned i = 0; i < children.size(); ++i)
			{
				if (auto comp = children[i]->owner.GetComponent<ProxyListener<EventObj>>())
				{
					if (comp->callback && comp->callback(e))
					{
						return true;
					}
				}
			}

			for (unsigned i = 0; i < children.size(); ++i)
			{
				if (Distribute(children[i]->owner, e))
				{
					return true;
				}
			}

			return false;
		}

		Listener<EventObj> listener;
	};
}
