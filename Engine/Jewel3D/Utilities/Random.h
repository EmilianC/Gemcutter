// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace Jwl
{
	void SeedRandomNumberGenerator();
	void SeedRandomNumberGenerator(unsigned seed);

	float RandomRangef(float min, float max);
	int RandomRangei(int min, int max);

	class RandomRange
	{
	public:
		RandomRange() = default;
		RandomRange(float value, float deviation);

		//- Potential range is value +- half of deviation.
		float Rand() const;
		//- Respecify range.
		void Set(float value, float deviation);

		float value = 0.0f;
		float deviation = 0.0f;
	};
}
