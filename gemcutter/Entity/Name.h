// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Entity/Entity.h"

#include <string>
#include <string_view>

namespace gem
{
	// Searches the given entity's sub-tree for the first child with the specified name.
	Entity* FindChild(const Entity& root, std::string_view name);

	// Searches all Entities with a name component and returns the first one found with the specified name.
	Entity* FindEntity(std::string_view name);

	// Associates an entity with a name.
	class Name : public Component<Name>
	{
	public:
		Name(Entity& owner);
		Name(Entity& owner, std::string name);

		std::string name;
	};
}
