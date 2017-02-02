// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Entity.h"

#include <vector>

namespace Jwl
{
	//- Represents a list of Entities to be associated together.
	class EntityGroup
	{
	public:
		void Add(Entity::Ptr ent);
		void Remove(const Entity& ent);

		bool Has(const Entity& ent) const;
		
		void Clear();

		const auto& GetEntities() const { return entities; }

	private:
		std::vector<Entity::Ptr> entities;
	};
}
