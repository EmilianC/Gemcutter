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
		srand(static_cast<u32>(time(NULL)));
	}

	void SeedRandomNumberGenerator(u32 seed)
	{
		srand(seed);
	}

	f32 RandomRange(f32 min, f32 max)
	{
		return min + ((max - min) * rand()) / (RAND_MAX + 1.0f);
	}

	s32 RandomRange(s32 min, s32 max)
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

	Range::Range(f32 _min, f32 _max)
	{
		Set(_min, _max);
	}

	Range Range::Deviation(f32 value, f32 deviation)
	{
		f32 halfRange = deviation * 0.5f;
		return Range(value - halfRange, value + halfRange);
	}

	f32 Range::Random() const
	{
		return RandomRange(min, max);
	}

	void Range::Set(f32 _min, f32 _max)
	{
		ASSERT(_min <= _max, "Invalid range.");
		min = _min;
		max = _max;
	}

	bool Range::Contains(f32 value) const
	{
		return value >= min && value <= max;
	}
}
