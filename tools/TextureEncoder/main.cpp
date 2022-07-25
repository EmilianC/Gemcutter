// Copyright (c) 2022 Emilian Cioca
#include "TextureEncoder.h"

int main()
{
	auto encoder = TextureEncoder();

	if (!gem::Encoder::RunEncoder(encoder))
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
