// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace Jwl
{
	class vec3;

	void SeedRandomNumberGenerator();
	void SeedRandomNumberGenerator(u32 seed);

	f32 RandomRange(f32 min, f32 max);
	s32 RandomRange(s32 min, s32 max);

	// Returns a random unit-length vector.
	vec3 RandomDirection();
	// Returns a random color with [0, 1] RGB values.
	vec3 RandomColor();

	class Range
	{
	public:
		Range() = default;
		Range(f32 min, f32 max);

		// Potential range is 'value' +- half the 'deviation'.
		static Range Deviation(f32 value, f32 deviation);

		f32 Random() const;
		void Set(f32 min, f32 max);

		bool Contains(f32 value) const;

		f32 min = 0.0f;
		f32 max = 1.0f;
	};
}
