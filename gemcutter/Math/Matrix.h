// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace Jwl
{
	struct vec2;
	struct vec3;
	struct vec4;
	struct mat4;
	struct quat;

	struct mat2
	{
		enum // Indexes
		{
			RightX = 0, UpX = 2,
			RightY = 1, UpY = 3
		};

		mat2();
		mat2(const vec2& right, const vec2& up);
		mat2(float f0, float f2,
			float f1, float f3);

		bool operator==(const mat2&) const;
		bool operator!=(const mat2&) const;

		mat2& operator*=(const mat2&);
		mat2& operator*=(float scalar);
		mat2& operator/=(float divisor);
		mat2& operator+=(const mat2&);
		mat2& operator-=(const mat2&);
		mat2 operator*(const mat2&) const;
		mat2 operator+(const mat2&) const;
		mat2 operator-(const mat2&) const;
		vec2 operator*(const vec2&) const;
		mat2 operator*(float scalar) const;
		mat2 operator/(float divisor) const;
		mat2 operator-() const;

		float operator[](unsigned index) const;
		float& operator[](unsigned index);

		void Transpose();
		void Inverse();
		mat2 GetTranspose() const;
		mat2 GetInverse() const;
		float GetDeterminant() const;

		void Scale(const vec2& scale);
		void Scale(float scale);

		void Rotate(float degrees);

		void SetRight(const vec2& V);
		void SetUp(const vec2& V);

		vec2 GetRight() const;
		vec2 GetUp() const;

		static const mat2 Identity;

		float data[4];
	};

	struct mat3
	{
		enum // Indexes
		{
			RightX = 0, UpX = 3, ForwardX = 6,
			RightY = 1, UpY = 4, ForwardY = 7,
			RightZ = 2, UpZ = 5, ForwardZ = 8
		};

		mat3();
		explicit mat3(const quat& rotation);
		// Extracts the rotational component from the 4x4 matrix.
		explicit mat3(const mat4& mat);
		mat3(const quat& rotation, const vec3& scale);
		mat3(const vec3& right, const vec3& up, const vec3& forward);
		mat3(float f0, float f3, float f6,
			float f1, float f4, float f7,
			float f2, float f5, float f8);

		bool operator==(const mat3&) const;
		bool operator!=(const mat3&) const;

		mat3& operator*=(const mat3&);
		mat3& operator*=(float scalar);
		mat3& operator/=(float divisor);
		mat3& operator+=(const mat3&);
		mat3& operator-=(const mat3&);
		mat3 operator*(const mat3&) const;
		mat3 operator+(const mat3&) const;
		mat3 operator-(const mat3&) const;
		vec3 operator*(const vec3&) const;
		mat3 operator*(float scalar) const;
		mat3 operator/(float divisor) const;
		mat3 operator-() const;

		float operator[](unsigned index) const;
		float& operator[](unsigned index);

		void Transpose();
		void Inverse();
		mat3 GetTranspose() const;
		mat3 GetInverse() const;
		float GetDeterminant() const;

		void Scale(const vec3& scale);
		void Scale(float scale);

		void Rotate(const vec3& axis, float degrees);
		void RotateX(float degrees);
		void RotateY(float degrees);
		void RotateZ(float degrees);

		void SetRight(const vec3& V);
		void SetUp(const vec3& V);
		void SetForward(const vec3& V);

		vec3 GetRight() const;
		vec3 GetUp() const;
		vec3 GetForward() const;

		static const mat3 Identity;

		// 'forward' must be a normalized vector.
		static mat3 LookAt(const vec3& forward, const vec3& upAnchor);

		float data[9];
	};

	struct mat4
	{
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
		mat4(const quat& rotation, const vec3& translation, const vec3& scale);
		mat4(const mat3& rotation, const vec3& translation, const vec3& scale);
		mat4(const vec3& right, const vec3& up, const vec3& forward, const vec3& translation);
		mat4(float f0, float f4, float f8, float f12,
			float f1, float f5, float f9, float f13,
			float f2, float f6, float f10, float f14,
			float f3, float f7, float f11, float f15);

		bool operator==(const mat4&) const;
		bool operator!=(const mat4&) const;

		mat4& operator*=(const mat4&);
		mat4& operator*=(float scalar);
		mat4& operator/=(float divisor);
		mat4& operator+=(const mat4&);
		mat4& operator-=(const mat4&);
		mat4 operator*(const mat4&) const;
		mat4 operator+(const mat4&) const;
		mat4 operator-(const mat4&) const;
		vec4 operator*(const vec4&) const;
		mat4 operator*(float scalar) const;
		mat4 operator/(float divisor) const;
		mat4 operator-() const;

		float operator[](unsigned index) const;
		float& operator[](unsigned index);

		void Transpose();
		void Inverse();
		// Computes the inverse assuming standard homogeneous matrix format.
		// [ R     T ]
		// [ 0 0 0 1 ]
		void FastInverse();
		mat4 GetTranspose() const;
		mat4 GetInverse() const;
		// Computes the inverse assuming standard homogeneous matrix format.
		// [ R     T ]
		// [ 0 0 0 1 ]
		mat4 GetFastInverse() const;

		void Scale(const vec3& scale);
		void Scale(float scale);

		void Rotate(const vec3& axis, float degrees);
		void RotateX(float degrees);
		void RotateY(float degrees);
		void RotateZ(float degrees);

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

		static mat4 PerspectiveProjection(float fovyDegrees, float aspect, float zNear, float zFar);
		static mat4 InversePerspectiveProjection(float fovyDegrees, float aspect, float zNear, float zFar);

		static mat4 OrthographicProjection(float left, float right, float top, float bottom, float zNear, float zFar);
		static mat4 InverseOrthographicProjection(float left, float right, float top, float bottom, float zNear, float zFar);

		static mat4 LookAt(const vec3& position, const vec3& target, const vec3& upAnchor);

		float data[16];
	};
}
