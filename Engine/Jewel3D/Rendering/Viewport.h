// Copyright (c) 2017 Emilian Cioca
#pragma once

#include "Jewel3D/Application/Types.h"

namespace Jwl
{
	class Viewport
	{
	public:
		Viewport() = default;
		Viewport(u32 x, u32 y, u32 width, u32 height);

		void bind() const;
		f32 GetAspectRatio() const;

		u32 x = 0;
		u32 y = 0;
		u32 width = 1;
		u32 height = 1;
	};
}
