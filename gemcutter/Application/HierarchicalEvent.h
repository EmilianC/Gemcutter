// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Application/Delegate.h"
#include "gemcutter/Application/Event.h"
#include "gemcutter/Entity/Entity.h"
#include "gemcutter/Entity/Hierarchy.h"

namespace gem
{
	// Responds to events given from an EventDispatcher component higher in the hierarchy.
	template<class EventObj>
	class HierarchicalListener : public Component<HierarchicalListener<EventObj>>
	{
		static_assert(std::is_base_of_v<EventBase, EventObj>, "Template argument must inherit from Event.");
	public:
		HierarchicalListener(Entity& _owner)
			: Component<HierarchicalListener<EventObj>>(_owner)
		{
		}

		// Your function should return 'true' if the event is handled. This will
		// consume it and stop it from propagating further down the hierarchy.
		Delegate<bool(const EventObj&)> callback;
	};

	// Propagates an event through a hierarchy of Entities.
	template<class EventObj>
	class HierarchicalDispatcher : public Component<HierarchicalDispatcher<EventObj>>
	{
		static_assert(std::is_base_of_v<EventBase, EventObj>, "Template argument must inherit from Event.");
	public:
		HierarchicalDispatcher(Entity& _owner)
			: Component<HierarchicalDispatcher<EventObj>>(_owner)
		{
			this->owner.Require<Hierarchy>();

			listener = [this](auto& e) {
				if (this->IsEnabled())
				{
					Distribute(this->owner, e);
				}
			};
		}

	private:
		// Propagates the event through the hierarchy while notifying HierarchicalListeners.
		// Once a listener has handled the event, propagation stops.
		static bool Distribute(Entity& ent, const EventObj& e)
		{
			auto& children = ent.Get<Hierarchy>().GetChildren();

			// Manual loop to avoid iterator invalidation from callback side-effects.
			for (unsigned i = 0; i < children.size(); ++i)
			{
				if (auto* comp = children[i]->Try<HierarchicalListener<EventObj>>())
				{
					if (comp->callback(e).value_or(false))
					{
						return true;
					}
				}
			}

			for (unsigned i = 0; i < children.size(); ++i)
			{
				if (Distribute(*children[i], e))
				{
					return true;
				}
			}

			return false;
		}

		Listener<EventObj> listener;
	};
}
