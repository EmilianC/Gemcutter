// Copyright (c) 2022 Emilian Cioca
#pragma once
#include <loupe/loupe.h>

namespace gem
{
	extern loupe::reflection_blob reflection_tables;

	// Populates the global reflection tables for the program.
	// Should only be called once at the top of main().
	void InitializeReflectionTables();
}
