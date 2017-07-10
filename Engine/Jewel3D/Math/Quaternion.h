// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace Jwl
{
	class vec3;
	class mat3;

	class quat
	{
	public:
		quat() = default;
		quat(f32 X, f32 Y, f32 Z, f32 W);
		quat(const vec3& right, const vec3& up, const vec3& forward);
		quat(const mat3& rotation);
		
		quat operator*(const quat&) const;
		vec3 operator*(const vec3&) const;
		quat& operator*=(const quat&);

		f32 operator[](u32 index) const;
		f32& operator[](u32 index);

		void SetIdentity();
		void Conjugate();
		quat GetConjugate() const;
		void Normalize();
		quat GetNormalized() const;

		vec3 GetRight() const;
		vec3 GetUp() const;
		vec3 GetForward() const;

		void Rotate(const vec3& axis, f32 degrees);
		void Rotate(f32 X, f32 Y, f32 Z, f32 degrees);
		void RotateX(f32 degrees);
		void RotateY(f32 degrees);
		void RotateZ(f32 degrees);
		
		static const quat Identity;

		f32 x = 0.0f;
		f32 y = 0.0f;
		f32 z = 0.0f;
		f32 w = 1.0f;
	};

	f32 Dot(const quat& p0, const quat& p1);
	quat Slerp(const quat& p0, const quat& p1, f32 percent);
}
