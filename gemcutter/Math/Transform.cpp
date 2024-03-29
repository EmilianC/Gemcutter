// Copyright (c) 2017 Emilian Cioca
#include "Transform.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Math.h"

#include <loupe/loupe.h>

namespace gem
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

	void Transform::LookAt(const vec3& pos, const vec3& target, const vec3& up)
	{
		ASSERT(Equals(Length(up), 1.0f), "'up' must be a normalized vector.");
		ASSERT(pos != target, "Transform cannot look at itself.");

		vec3 forward = Normalize(target - pos);
		vec3 right = Normalize(Cross(forward, up));

		rotation = quat(right, Cross(right, forward), -forward);
		position = pos;
	}

	void Transform::Rotate(const vec3& axis, float degrees)
	{
		rotation.Rotate(axis, degrees);
	}

	void Transform::RotateX(float degrees)
	{
		rotation.RotateX(degrees);
	}

	void Transform::RotateY(float degrees)
	{
		rotation.RotateY(degrees);
	}

	void Transform::RotateZ(float degrees)
	{
		rotation.RotateZ(degrees);
	}
}

REFLECT(gem::Transform)
	MEMBERS {
		REF_MEMBER(position)
		REF_MEMBER(rotation)
		REF_MEMBER(scale)
	}
REF_END;
