// Copyright (c) 2022 Emilian Cioca
#include "Reflection.h"
#include "gemcutter/Entity/Entity.h"

namespace gem
{
	namespace detail
	{
		extern const loupe::type* componentBaseType;
	}

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

		detail::componentBaseType = &ReflectType<ComponentBase>();
	}
}

REFLECT_SIMPLE(gem::ReadOnly);
REFLECT_SIMPLE(gem::Hidden);
