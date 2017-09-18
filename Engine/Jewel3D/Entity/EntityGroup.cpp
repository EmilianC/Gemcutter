// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "EntityGroup.h"
#include "Jewel3D/Application/Logging.h"

namespace Jwl
{
	void EntityGroup::Add(Entity::Ptr ent)
	{
		ASSERT(ent, "Cannot add a null Entity to an EntityGroup.");
		ASSERT(!Has(*ent), "Entity is already part of the EntityGroup.");

		entities.push_back(ent);
	}

	void EntityGroup::Remove(const Entity& ent)
	{
		for (unsigned i = 0; i < entities.size(); ++i)
		{
			if (entities[i].get() == &ent)
			{
				entities.erase(entities.begin() + i);
				return;
			}
		}
	}

	bool EntityGroup::Has(const Entity& ent) const
	{
		for (auto& entity : entities)
		{
			if (entity.get() == &ent)
			{
				return true;
			}
		}

		return false;
	}

	void EntityGroup::Clear()
	{
		entities.clear();
	}
}
