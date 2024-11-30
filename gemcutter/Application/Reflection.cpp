// Copyright (c) 2022 Emilian Cioca
#include "Reflection.h"

namespace
{
	std::vector<const loupe::type*> componentTypes;
}

namespace gem
{
	loupe::reflection_blob reflection_tables;

	const loupe::type* EntityTypeId = nullptr;
	const loupe::type* BaseComponentTypeId = nullptr;
	const loupe::type* BaseResourceTypeId = nullptr;

	void InitializeReflectionTables()
	{
#ifdef GEM_DEBUG
		constinit static bool calledOnce = false;
		ASSERT(!calledOnce, "InitializeReflectionTables() should not be called multiple times.");
		calledOnce = true;
#endif
		reflection_tables = loupe::reflect(1);
		loupe::clear_reflect_tasks();

		EntityTypeId        = reflection_tables.find<class Entity>();
		BaseComponentTypeId = reflection_tables.find<class ComponentBase>();
		BaseResourceTypeId  = reflection_tables.find<class ResourceBase>();

		// Cache all component types. They will already be sorted alphabetically by loupe.
		componentTypes.reserve(64);
		for (const loupe::type& type : reflection_tables.get_types())
		{
			// The base component itself is not required.
			if (&type == BaseComponentTypeId) [[unlikely]]
				continue;

			if (type.is_a(*BaseComponentTypeId))
			{
				componentTypes.push_back(&type);
			}
		}
		componentTypes.shrink_to_fit();
	}

	std::string_view GetDisplayName(const loupe::member& member)
	{
		if (auto* displayName = member.metadata.find<loupe::metadata::display_name>())
		{
			return displayName->name;
		}

		return member.name;
	}

	std::string_view GetDisplayName(const loupe::enum_entry& entry)
	{
		if (auto* displayName = entry.metadata.find<loupe::metadata::display_name>())
		{
			return displayName->name;
		}

		return entry.name;
	}

	std::span<const loupe::type*> GetAllComponentTypes()
	{
		return componentTypes;
	}
}
