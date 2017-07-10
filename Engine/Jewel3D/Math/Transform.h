// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Vector.h"
#include "Quaternion.h"

namespace Jwl
{
	//- A pose in space - Position / Rotation / Scale.
	class Transform
	{
	public:
		Transform() = default;
		Transform(const vec3& position);
		Transform(const vec3& position, const quat& rotation);
		Transform(const vec3& position, const quat& rotation, const vec3& scale);

		void LookAt(const vec3& pos, const vec3& target, const vec3& up = vec3::Up);
		void Rotate(const vec3& axis, f32 degrees);
		void RotateX(f32 degrees);
		void RotateY(f32 degrees);
		void RotateZ(f32 degrees);

		vec3 position;
		quat rotation;
		vec3 scale = vec3::One;
	};
}
