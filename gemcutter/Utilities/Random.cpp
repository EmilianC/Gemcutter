// Copyright (c) 2017 Emilian Cioca
#include "Random.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Math.h"
#include "gemcutter/Math/Vector.h"

#include <cstdlib>
#include <loupe/loupe.h>
#include <random>

namespace
{
	std::mt19937 engine;
}

namespace gem
{
	void SeedRandomNumberGenerator()
	{
		std::random_device rd;

		engine.seed(rd());
		srand(static_cast<unsigned>(time(nullptr)));
	}

	void SeedRandomNumberGenerator(unsigned seed)
	{
		engine.seed(seed);
		srand(seed);
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

	bool RandomBool(float probability)
	{
		ASSERT(probability >= 0.0f && probability <= 1.0f, "Probability must be within [0, 1].");

		std::bernoulli_distribution dist(probability);
		return dist(engine);
	}

	Range::Range(float _min, float _max)
	{
		Set(_min, _max);
	}

	Range Range::Deviation(float value, float deviation)
	{
		float halfRange = Abs(deviation) * 0.5f;
		return { value - halfRange, value + halfRange };
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

REFLECT(gem::Range)
	MEMBERS {
		REF_MEMBER(min)
		REF_MEMBER(max)
	}
REF_END;
