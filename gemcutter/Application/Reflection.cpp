// Copyright (c) 2022 Emilian Cioca
#include "Reflection.h"
#include "gemcutter/Entity/Entity.h"
#include "gemcutter/Resource/Resource.h"

namespace
{
	std::vector<const loupe::type*> componentTypes;
	std::vector<const loupe::type*> tagTypes;
}

namespace gem
{
	loupe::reflection_blob reflection_tables;

	const loupe::type* EntityTypeId = nullptr;
	const loupe::type* BaseTagTypeId = nullptr;
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

		EntityTypeId        = reflection_tables.find<Entity>();
		BaseTagTypeId       = reflection_tables.find<TagBase>();
		BaseComponentTypeId = reflection_tables.find<ComponentBase>();
		BaseResourceTypeId  = reflection_tables.find<ResourceBase>();

		// Cache all component & tag types. They will already be sorted alphabetically by loupe.
		componentTypes.reserve(64);
		tagTypes.reserve(16);
		for (const loupe::type& type : reflection_tables.get_types())
		{
			if (type.is_a(*BaseTagTypeId))
			{
				// The literal base type itself is not required.
				if (&type == BaseTagTypeId) [[unlikely]]
					continue;
			
				tagTypes.push_back(&type);
			}
			else if (type.is_a(*BaseComponentTypeId))
			{
				if (&type == BaseComponentTypeId) [[unlikely]]
					continue;

				componentTypes.push_back(&type);
			}
		}
		componentTypes.shrink_to_fit();
		tagTypes.shrink_to_fit();
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

	std::span<const loupe::type*> GetAllTagTypes()
	{
		return tagTypes;
	}
}
