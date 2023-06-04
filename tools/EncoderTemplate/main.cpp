#include "Encoder.h"

#include <gemcutter/Application/Reflection.h>

int main()
{
	gem::InitializeReflectionTables();

	auto encoder = Encoder();

	if (!gem::Encoder::RunEncoder(encoder))
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
