// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Random.h"
#include "Jewel3D/Math/Vector.h"

#include <stdlib.h>
#include <time.h>

namespace Jwl
{
	void SeedRandomNumberGenerator()
	{
		srand(static_cast<unsigned>(time(NULL)));
	}

	void SeedRandomNumberGenerator(unsigned seed)
	{
		srand(seed);
	}

	float RandomRangef(float min, float max)
	{
		return min + ((max - min) * rand()) / (RAND_MAX + 1.0f);
	}

	int RandomRangei(int min, int max)
	{
		return rand() % (max + 1 - min) + min;
	}
	
	vec3 RandomDirection()
	{
		return vec3(
			RandomRangef(-1.0f, 1.0f),
			RandomRangef(-1.0f, 1.0f),
			RandomRangef(-1.0f, 1.0f)).GetNormalized();
	}

	RandomRange::RandomRange(float _value, float _deviation)
		: value(_value)
		, deviation(_deviation)
	{
	}

	float RandomRange::Rand() const
	{
		float range = deviation * 0.5f;
		return RandomRangef(value - range, value + range);
	}

	void RandomRange::Set(float _value, float _deviation)
	{
		value = _value;
		deviation = _deviation;
	}
}
