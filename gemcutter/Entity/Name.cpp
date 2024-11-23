// Copyright (c) 2017 Emilian Cioca
#include "Name.h"
#include "gemcutter/Entity/Hierarchy.h"

namespace gem
{
	Entity* FindChild(const Entity& root, std::string_view name)
	{
		if (name.empty())
		{
			return nullptr;
		}

		auto& hierarchy = root.Get<Hierarchy>();

		for (auto& child : hierarchy.GetChildren())
		{
			auto* nameComp = child->Try<Name>();
			if (nameComp && nameComp->name == name)
			{
				return child.get();
			}
		}

		for (auto& child : hierarchy.GetChildren())
		{
			if (auto* result = FindChild(*child, name))
			{
				return result;
			}
		}

		return nullptr;
	}

	Entity* FindEntity(std::string_view name)
	{
		if (name.empty())
		{
			return nullptr;
		}

		for (auto& comp : All<Name>())
		{
			if (comp.name == name)
			{
				return &comp.owner;
			}
		}

		return nullptr;
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
}

REFLECT_COMPONENT(gem::Name, gem::ComponentBase)
	MEMBERS {
		REF_MEMBER(name)
	}
REF_END;
