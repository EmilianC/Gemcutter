// Copyright (c) 2017 Emilian Cioca
#include "Viewport.h"
#include "gemcutter/Rendering/Rendering.h"

namespace gem
{
	void Viewport::bind() const
	{
		SetViewport(x, y, width, height);
	}

	float Viewport::GetAspectRatio() const
	{
		return static_cast<float>(width) / static_cast<float>(height);
	}
}
