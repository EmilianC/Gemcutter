// Copyright (c) 2022 Emilian Cioca
#include "Reflection.h"
#include "gemcutter/Entity/Entity.h"
#include "gemcutter/Resource/Resource.h"

namespace gem
{
	const loupe::type* EntityTypeId = nullptr;
	const loupe::type* BaseComponentTypeId = nullptr;
	const loupe::type* BaseResourceTypeId = nullptr;

	loupe::reflection_blob reflection_tables;

	void InitializeReflectionTables()
	{
#ifdef GEM_DEBUG
		constinit static bool calledOnce = false;
		ASSERT(!calledOnce, "InitializeReflectionTables() should not be called multiple times.");
		calledOnce = true;
#endif
		reflection_tables = loupe::reflect(1);
		loupe::clear_reflect_tasks();

		EntityTypeId        = &ReflectType<Entity>();
		BaseComponentTypeId = &ReflectType<ComponentBase>();
		BaseResourceTypeId  = &ReflectType<ResourceBase>();
	}
}

REFLECT_SIMPLE(gem::ReadOnly);
REFLECT_SIMPLE(gem::Hidden);
