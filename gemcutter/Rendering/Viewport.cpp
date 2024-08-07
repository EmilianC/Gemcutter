// Copyright (c) 2017 Emilian Cioca
#include "Viewport.h"
#include "gemcutter/Rendering/Rendering.h"

#include <loupe/loupe.h>
#include <loupe/metadata.h>

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

REFLECT(gem::Viewport)
	MEMBERS {
		REF_MEMBER(x)
		REF_MEMBER(y)
		REF_MEMBER(width,  min(0u))
		REF_MEMBER(height, min(0u))
	}
REF_END;
