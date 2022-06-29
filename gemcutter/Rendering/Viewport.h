// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace gem
{
	struct Viewport
	{
		void bind() const;
		float GetAspectRatio() const;

		unsigned x = 0;
		unsigned y = 0;
		unsigned width = 1;
		unsigned height = 1;
	};
}
