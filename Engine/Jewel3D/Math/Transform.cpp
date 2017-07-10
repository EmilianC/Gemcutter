// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Transform.h"
#include "Math.h"
#include "Jewel3D/Application/Logging.h"

#include <cfloat>

namespace Jwl
{
	Transform::Transform(const vec3& _position)
		: position(_position)
	{
	}

	Transform::Transform(const vec3& _position, const quat& _rotation)
		: position(_position)
		, rotation(_rotation)
	{
	}

	Transform::Transform(const vec3& _position, const quat& _rotation, const vec3& _scale)
		: position(_position)
		, rotation(_rotation)
		, scale(_scale)
	{
	}

	void Transform::LookAt(const vec3 &pos, const vec3 &target, const vec3 &up)
	{
		ASSERT(Abs(up.Length() - 1.0f) < 0.0001f, "'up' must be a normalized vector.");
		ASSERT(pos != target, "Transform cannot look at itself.");

		vec3 forward = (target - pos).GetNormalized();
		vec3 right = Cross(forward, up).GetNormalized();

		rotation = quat(right, Cross(right, forward), -forward);
		position = pos;
	}

	void Transform::Rotate(const vec3 &axis, f32 degrees)
	{
		rotation.Rotate(axis, degrees);
	}

	void Transform::RotateX(f32 degrees)
	{
		rotation.RotateX(degrees);
	}

	void Transform::RotateY(f32 degrees)
	{
		rotation.RotateY(degrees);
	}

	void Transform::RotateZ(f32 degrees)
	{
		rotation.RotateZ(degrees);
	}
}
