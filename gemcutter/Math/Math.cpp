// Copyright (c) 2017 Emilian Cioca
#include "Math.h"

namespace gem
{
	unsigned CombineHashes(unsigned a, unsigned b)
	{
		return a ^ (b + 0x9E3779B9 + (a << 6) + (a >> 2));
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
