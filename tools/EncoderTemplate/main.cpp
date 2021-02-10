#include "Encoder.h"

int main()
{
	auto encoder = Encoder();

	if (!gem::Encoder::RunEncoder(encoder))
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
