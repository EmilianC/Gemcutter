// Copyright (c) 2017 Emilian Cioca
#include "Random.h"
#include "gemcutter/Math/Vector.h"

#include <random>

namespace gem
{
	std::mt19937 engine;

	void SeedRandomNumberGenerator()
	{
		std::random_device rd;
		engine.seed(rd());
	}

	void SeedRandomNumberGenerator(unsigned seed)
	{
		engine.seed(seed);
	}

	float RandomRange(float min, float max)
	{
		std::uniform_real_distribution<float> dist(min, std::nextafter(max, FLT_MAX));
		return dist(engine);
	}

	int RandomRange(int min, int max)
	{
		std::uniform_int_distribution<int> dist(min, max);
		return dist(engine);
	}

	vec3 RandomDirection()
	{
		return Normalize(vec3(
			RandomRange(-1.0f, 1.0f),
			RandomRange(-1.0f, 1.0f),
			RandomRange(-1.0f, 1.0f)));
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
