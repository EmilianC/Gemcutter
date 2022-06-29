// Copyright (c) 2021 Emilian Cioca
#pragma once

namespace gem
{
	struct vec2;

	struct Rectangle
	{
		float GetAspectRatio() const;
		bool Contains(float x, float y) const;
		bool Contains(const vec2& pos) const;

		float x = 0.0f;
		float y = 0.0f;
		float width = 1.0f;
		float height = 1.0f;
	};
}
