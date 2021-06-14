// Copyright (c) 2017 Emilian Cioca
#include "Math.h"

namespace gem
{
	unsigned CombineHashes(unsigned a, unsigned b)
	{
		return a ^ (b + 0x9E3779B9 + (a << 6) + (a >> 2));
	}

	unsigned NextPowerOfTwo(unsigned value)
	{
		if (value == 0)
			return 1;

		value--;
		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;
		value++;

		return value;
	}

	float SnapToGrid(float value, float step)
	{
		return std::floorf((value + (step / 2.0f)) / step) * step;
	}

	float EaseIn(float percent)
	{
		return 1.0f - cosf(percent * (M_PI / 2.0f));
	}

	float EaseOut(float percent)
	{
		return cosf((percent - 1.0f) * (M_PI / 2.0f));
	}
}
