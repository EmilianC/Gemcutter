// Copyright (c) 2022 Emilian Cioca
#include "Reflection.h"

namespace gem
{
	loupe::reflection_blob reflection_tables;

	void InitializeReflectionTables()
	{
		reflection_tables = loupe::reflect();
		loupe::clear_reflect_tasks();
	}
}
