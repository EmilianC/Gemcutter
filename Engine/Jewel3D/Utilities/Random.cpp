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

	float RandomRange(float min, float max)
	{
		return min + ((max - min) * rand()) / (RAND_MAX + 1.0f);
	}

	int RandomRange(int min, int max)
	{
		return rand() % (max + 1 - min) + min;
	}
	
	vec3 RandomDirection()
	{
		return vec3(
			RandomRange(-1.0f, 1.0f),
			RandomRange(-1.0f, 1.0f),
			RandomRange(-1.0f, 1.0f)).GetNormalized();
	}

	vec3 RandomColor()
	{
		return vec3(
			RandomRange(0.0f, 1.0f),
			RandomRange(0.0f, 1.0f),
			RandomRange(0.0f, 1.0f));
	}

	Range::Range(float _min, float _max)
	{
		Set(_min, _max);
	}

	Range Range::Deviation(float value, float deviation)
	{
		float halfRange = deviation * 0.5f;
		return Range(value - halfRange, value + halfRange);
	}

	float Range::Random() const
	{
		return RandomRange(min, max);
	}

	void Range::Set(float _min, float _max)
	{
		ASSERT(_min <= _max, "Invalid range.");
		min = _min;
		max = _max;
	}

	bool Range::Contains(float value) const
	{
		return value >= min && value <= max;
	}
}
