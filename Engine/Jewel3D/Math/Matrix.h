// Copyright (c) 2017 Emilian Cioca
#pragma once

#include "Jewel3D/Application/Types.h"

namespace Jwl
{
	class vec2;
	class vec3;
	class vec4;
	class mat4;
	class quat;

	class mat2
	{
	public:
		enum // Indexes
		{
			RightX = 0, UpX = 2,
			RightY = 1, UpY = 3
		};

		mat2();
		mat2(const vec2& right, const vec2& up);
		mat2(f32 f0, f32 f2,
			f32 f1, f32 f3);

		mat2& operator=(const mat2&);
		mat2& operator*=(const mat2&);
		mat2& operator*=(f32 scalar);
		mat2& operator/=(f32 divisor);
		mat2& operator+=(const mat2&);
		mat2& operator-=(const mat2&);
		mat2 operator*(const mat2&) const;
		mat2 operator+(const mat2&) const;
		mat2 operator-(const mat2&) const;
		vec2 operator*(const vec2&) const;
		mat2 operator*(f32 scalar) const;
		mat2 operator/(f32 divisor) const;
		mat2 operator-() const;

		f32 operator[](u32 index) const;
		f32& operator[](u32 index);

		void Transpose();
		void Inverse();
		mat2 GetTranspose() const;
		mat2 GetInverse() const;
		f32 GetDeterminant() const;

		void Scale(const vec2& scale);
		void Scale(f32 scale);

		void Rotate(f32 degrees);

		void SetRight(const vec2& V);
		void SetUp(const vec2& V);

		vec2 GetRight() const;
		vec2 GetUp() const;

		static const mat2 Identity;

		f32 data[4];
	};

	class mat3
	{
	public:
		enum // Indexes
		{
			RightX = 0, UpX = 3, ForwardX = 6,
			RightY = 1, UpY = 4, ForwardY = 7,
			RightZ = 2, UpZ = 5, ForwardZ = 8
		};

		mat3();
		explicit mat3(const quat& rotation);
		//- Extracts the rotational component from the 4x4 matrix.
		explicit mat3(const mat4& mat);
		mat3(const vec3& right, const vec3& up, const vec3& forward);
		mat3(f32 f0, f32 f3, f32 f6,
			f32 f1, f32 f4, f32 f7,
			f32 f2, f32 f5, f32 f8);

		mat3& operator=(const mat3&);
		mat3& operator*=(const mat3&);
		mat3& operator*=(f32 scalar);
		mat3& operator/=(f32 divisor);
		mat3& operator+=(const mat3&);
		mat3& operator-=(const mat3&);
		mat3 operator*(const mat3&) const;
		mat3 operator+(const mat3&) const;
		mat3 operator-(const mat3&) const;
		vec3 operator*(const vec3&) const;
		mat3 operator*(f32 scalar) const;
		mat3 operator/(f32 divisor) const;
		mat3 operator-() const;

		f32 operator[](u32 index) const;
		f32& operator[](u32 index);

		void Transpose();
		void Inverse();
		mat3 GetTranspose() const;
		mat3 GetInverse() const;
		f32 GetDeterminant() const;

		void Scale(const vec3& scale);
		void Scale(f32 scale);

		void Rotate(const vec3& axis, f32 degrees);
		void RotateX(f32 degrees);
		void RotateY(f32 degrees);
		void RotateZ(f32 degrees);

		void SetRight(const vec3& V);
		void SetUp(const vec3& V);
		void SetForward(const vec3& V);

		vec3 GetRight() const;
		vec3 GetUp() const;
		vec3 GetForward() const;

		static const mat3 Identity;

		f32 data[9];
	};

	class mat4
	{
	public:
		enum // Indexes
		{
			RightX = 0, UpX = 4, ForwardX = 8,  TransX = 12,
			RightY = 1, UpY = 5, ForwardY = 9,  TransY = 13,
			RightZ = 2, UpZ = 6, ForwardZ = 10, TransZ = 14,
			W0     = 3, W1  = 7, W2       = 11, W3   = 15
		};

		mat4();
		explicit mat4(const quat& rotation);
		explicit mat4(const mat3& rotation);
		mat4(const quat& rotation, const vec3& translation);
		mat4(const mat3& rotation, const vec3& translation);
		mat4(const vec3& right, const vec3& up, const vec3& forward, const vec4& translation);
		mat4(f32 f0, f32 f4, f32 f8, f32 f12,
			f32 f1, f32 f5, f32 f9, f32 f13,
			f32 f2, f32 f6, f32 f10, f32 f14,
			f32 f3, f32 f7, f32 f11, f32 f15);

		mat4& operator=(const mat4&);
		mat4& operator*=(const mat4&);
		mat4& operator*=(f32 scalar);
		mat4& operator/=(f32 divisor);
		mat4& operator+=(const mat4&);
		mat4& operator-=(const mat4&);
		mat4 operator*(const mat4&) const;
		mat4 operator+(const mat4&) const;
		mat4 operator-(const mat4&) const;
		vec4 operator*(const vec4&) const;
		mat4 operator*(f32 scalar) const;
		mat4 operator/(f32 divisor) const;
		mat4 operator-() const;

		f32 operator[](u32 index) const;
		f32& operator[](u32 index);

		void Transpose();
		void Inverse();
		//- Computes the inverse assuming standard homogeneous matrix format.
		//- [ R		T ]
		//- [ 0 0 0 1 ]
		void FastInverse();
		mat4 GetTranspose() const;
		mat4 GetInverse() const;
		//- Computes the inverse assuming standard homogeneous matrix format.
		//- [ R		T ]
		//- [ 0 0 0 1 ]
		mat4 GetFastInverse() const;

		void Scale(const vec3& scale);
		void Scale(f32 scale);

		void Rotate(const vec3& axis, f32 degrees);
		void RotateX(f32 degrees);
		void RotateY(f32 degrees);
		void RotateZ(f32 degrees);

		void Translate(const vec3& translation);

		void SetRight(const vec3& v);
		void SetUp(const vec3& v);
		void SetForward(const vec3& v);
		void SetTranslation(const vec3& v);

		vec3 GetRight() const;
		vec3 GetUp() const;
		vec3 GetForward() const;
		vec3 GetTranslation() const;

		static const mat4 Identity;
		
		static mat4 PerspectiveProjection(f32 fovyDegrees, f32 aspect, f32 zNear, f32 zFar);
		static mat4 InversePerspectiveProjection(f32 fovyDegrees, f32 aspect, f32 zNear, f32 zFar);
		
		static mat4 OrthographicProjection(f32 left, f32 right, f32 top, f32 bottom, f32 zNear, f32 zFar);
		static mat4 InverseOrthographicProjection(f32 left, f32 right, f32 top, f32 bottom, f32 zNear, f32 zFar);

		static mat4 LookAt(const vec3& position, const vec3& target, const vec3& upVector);

		f32 data[16];
	};
}
