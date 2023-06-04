// Copyright (c) 2022 Emilian Cioca
#include "TextureEncoder.h"

#include <gemcutter/Application/Reflection.h>

int main()
{
	gem::InitializeReflectionTables();

	auto encoder = TextureEncoder();

	if (!gem::Encoder::RunEncoder(encoder))
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
