// Copyright (c) 2017 Emilian Cioca
#include "FontEncoder.h"

#include <gemcutter/Application/Reflection.h>

int main()
{
	gem::InitializeReflectionTables();

	auto encoder = FontEncoder();

	if (!gem::Encoder::RunEncoder(encoder))
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
