// Copyright (c) 2017 Emilian Cioca
#include "MeshEncoder.h"

#include <gemcutter/Application/Reflection.h>

int main()
{
	gem::InitializeReflectionTables();

	auto encoder = MeshEncoder();

	if (!gem::Encoder::RunEncoder(encoder))
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
