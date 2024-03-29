// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace gem
{
	struct vec3;

	void SeedRandomNumberGenerator();
	void SeedRandomNumberGenerator(unsigned seed);

	// Returns a random float in the range [min, max].
	float RandomRange(float min, float max);
	// Returns a random int in the range [min, max].
	int RandomRange(int min, int max);

	// Returns a random unit-length vector.
	vec3 RandomDirection();
	// Returns a random color with [0, 1] RGB values.
	vec3 RandomColor();

	// Returns true randomly given the [0, 1] probability.
	bool RandomBool(float probability);

	struct Range
	{
		Range() = default;
		Range(float min, float max);

		// Potential range is the 'value' +/- half the 'deviation'.
		[[nodiscard]] static Range Deviation(float value, float deviation);

		float Random() const;
		void Set(float min, float max);

		bool Contains(float value) const;

		float min = 0.0f;
		float max = 1.0f;
	};
}
