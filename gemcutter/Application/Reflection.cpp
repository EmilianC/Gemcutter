// Copyright (c) 2022 Emilian Cioca
#include "Reflection.h"

namespace gem
{
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
	}
}

REFLECT_SIMPLE(gem::ReadOnly);
REFLECT_SIMPLE(gem::Hidden);
