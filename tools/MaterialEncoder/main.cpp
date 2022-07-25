// Copyright (c) 2022 Emilian Cioca
#include "MaterialEncoder.h"

int main()
{
	auto encoder = MaterialEncoder();

	if (!gem::Encoder::RunEncoder(encoder))
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
