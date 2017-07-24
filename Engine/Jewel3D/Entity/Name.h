// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"

#include <string>

namespace Jwl
{
	// Writes the sub-tree of Entity names to the program log.
	void LogSceneGraph(const Entity& root);

	// Searches the given entity's sub-tree for the first child with the specified name.
	Entity::Ptr FindChild(const Entity& root, const std::string& name);

	// Searches all Entities with a name component and returns the first one found with the specified name.
	Entity::Ptr FindEntity(const std::string& name);

	// Associates an entity with a name.
	// When copied, appends "_Copy" to the name.
	class Name : public Component<Name>
	{
	public:
		Name(Entity& owner);
		Name(Entity& owner, const std::string& name);
		Name& operator=(const Name&);

		std::string name;
	};
}
