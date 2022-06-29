// Copyright (c) 2021 Emilian Cioca
#include "Rectangle.h"

namespace gem
{
	float Rectangle::GetAspectRatio() const
	{
		return width / height;
	}

	bool Rectangle::Contains(float _x, float _y) const
	{
		return _x >= x &&
			_y >= y &&
			_x <= x + width &&
			_y <= y + height;
	}

	bool Rectangle::Contains(const vec2& pos) const
	{
		return Contains(pos.x, pos.y);
	}
}
