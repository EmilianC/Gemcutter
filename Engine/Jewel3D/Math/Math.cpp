// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Math.h"

namespace Jwl
{
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
