#include "MaterialEncoder.h"

int main()
{
	auto encoder = MaterialEncoder();

	if (!Jwl::Encoder::RunEncoder(encoder))
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
