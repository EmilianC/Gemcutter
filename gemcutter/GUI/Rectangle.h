// Copyright (c) 2021 Emilian Cioca
#pragma once

struct vec2;

namespace gem
{
	struct Rectangle
	{
		Rectangle() = default;
		Rectangle(float x, float y, float width, float height);
		Rectangle(const vec2& bottomLeft, float width, float height);

		float GetAspectRatio() const;
		bool Contains(float x, float y) const;
		bool Contains(const vec2& pos) const;

		float x = 0.0f;
		float y = 0.0f;
		float width = 1.0f;
		float height = 1.0f;
	};
}
