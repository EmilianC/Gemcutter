// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Name.h"

namespace Jwl
{
	void LogSceneGraphRecursive(const Entity& entity, unsigned tabLevel)
	{
		std::string output;
		output.append(tabLevel * 2, ' ');

		if (auto nameComp = entity.Try<Name>())
		{
			output += "|- " + nameComp->name;
		}
		else
		{
			output += "|- NO_NAME";
		}

		Log(output);

		for (auto& child : entity.GetChildren())
		{
			LogSceneGraphRecursive(*child, tabLevel + 1);
		}
	}

	void LogSceneGraph(const Entity& root)
	{
		LogSceneGraphRecursive(root, 0);
	}

	Entity::Ptr FindChild(const Entity& root, std::string_view name)
	{
		for (auto& child : root.GetChildren())
		{
			auto nameComp = child->Try<Name>();
			if (nameComp && nameComp->name == name)
			{
				return child;
			}
		}
		
		for (auto& child : root.GetChildren())
		{
			if (auto result = FindChild(*child, name))
			{
				return result;
			}
		}

		return nullptr;
	}

	Entity::Ptr FindEntity(std::string_view name)
	{
		Entity::Ptr result;

		for (auto& comp : All<Name>())
		{
			if (comp.name == name)
			{
				result = comp.owner.GetPtr();
				break;
			}
		}

		return result;
	}

	Name::Name(Entity& _owner)
		: Component(_owner)
	{
	}

	Name::Name(Entity& _owner, std::string _name)
		: Component(_owner)
		, name(std::move(_name))
	{
	}

	Name& Name::operator=(const Name& other)
	{
		name = other.name + "_Copy";

		return *this;
	}
}
