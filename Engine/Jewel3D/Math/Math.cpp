// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Math.h"

namespace Jwl
{
	f32 SnapToGrid(f32 value, f32 step)
	{
		return std::floorf((value + (step / 2.0f)) / step) * step;
	}

	f32 EaseIn(f32 percent)
	{
		return 1.0f - cosf(percent * (M_PI / 2.0f));
	}

	f32 EaseOut(f32 percent)
	{
		return cosf((percent - 1.0f) * (M_PI / 2.0f));
	}
}
