// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Rendering.h"
#include "Viewport.h"

namespace Jwl
{
	Viewport::Viewport(u32 _x, u32 _y, u32 _width, u32 _height)
		: x(_x)
		, y(_y)
		, width(_width)
		, height(_height)
	{
	}

	void Viewport::bind() const
	{
		SetViewport(x, y, width, height);
	}

	f32 Viewport::GetAspectRatio() const
	{
		return static_cast<f32>(width) / static_cast<f32>(height);
	}
}
