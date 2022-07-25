// Copyright (c) 2022 Emilian Cioca
#include "SoundEncoder.h"

int main()
{
	auto encoder = SoundEncoder();

	if (!gem::Encoder::RunEncoder(encoder))
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
