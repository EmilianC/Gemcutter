// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace Jwl
{
	class vec3;

	void SeedRandomNumberGenerator();
	void SeedRandomNumberGenerator(unsigned seed);

	float RandomRange(float min, float max);
	int RandomRange(int min, int max);
	vec3 RandomDirection();

	class Range
	{
	public:
		Range() = default;
		Range(float min, float max);

		// Potential range is 'value' +- half the 'deviation'.
		static Range Deviation(float value, float deviation);

		float Random() const;
		void Set(float min, float max);

		bool Contains(float value) const;

		float min = 0.0f;
		float max = 1.0f;
	};
}
