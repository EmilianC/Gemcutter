// Copyright (c) 2017 Emilian Cioca
#include "Matrix.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Math.h"
#include "gemcutter/Math/Quaternion.h"
#include "gemcutter/Math/Vector.h"

namespace gem
{
	const mat2 mat2::Identity = mat2();

	mat2::mat2()
	{
		data[0] = 1.0f;
		data[1] = 0.0f;

		data[2] = 0.0f;
		data[3] = 1.0f;
	}

	mat2::mat2(const vec2& right, const vec2& up)
	{
		data[RightX] = right.x;
		data[RightY] = right.y;

		data[UpX] = up.x;
		data[UpY] = up.y;
	}

	mat2::mat2(float f0, float f2, float f1, float f3)
	{
		data[0] = f0;
		data[1] = f1;

		data[2] = f2;
		data[3] = f3;
	}

	bool mat2::operator==(const mat2& M) const
	{
		return
			Equals(data[0], M.data[0]) &&
			Equals(data[1], M.data[1]) &&
			Equals(data[2], M.data[2]) &&
			Equals(data[3], M.data[3]);
	}

	bool mat2::operator!=(const mat2& M) const
	{
		return
			!Equals(data[0], M.data[0]) ||
			!Equals(data[1], M.data[1]) ||
			!Equals(data[2], M.data[2]) ||
			!Equals(data[3], M.data[3]);
	}

	mat2& mat2::operator*=(const mat2& M)
	{
		return *this = (*this) * M;
	}

	mat2& mat2::operator*=(float scalar)
	{
		data[0] *= scalar;
		data[1] *= scalar;

		data[2] *= scalar;
		data[3] *= scalar;

		return *this;
	}

	mat2& mat2::operator/=(float divisor)
	{
		float inverse = 1.0f / divisor;

		data[0] *= inverse;
		data[1] *= inverse;

		data[2] *= inverse;
		data[3] *= inverse;

		return *this;
	}

	mat2& mat2::operator+=(const mat2& M)
	{
		data[0] += M.data[0];
		data[1] += M.data[1];

		data[2] += M.data[2];
		data[3] += M.data[3];

		return *this;
	}

	mat2& mat2::operator-=(const mat2& M)
	{
		data[0] -= M.data[0];
		data[1] -= M.data[1];

		data[2] -= M.data[2];
		data[3] -= M.data[3];

		return *this;
	}

	mat2 mat2::operator*(const mat2& M) const
	{
		return mat2(
			data[0] + data[2], data[0] + data[2],
			data[1] + data[3], data[1] + data[3]);
	}

	mat2 mat2::operator+(const mat2& M) const
	{
		return mat2(
			data[0] + M.data[0], data[2] + M.data[2],
			data[1] + M.data[1], data[3] + M.data[3]);
	}

	mat2 mat2::operator-(const mat2& M) const
	{
		return mat2(
			data[0] * M.data[0] + data[2] * M.data[1], data[0] * M.data[2] + data[2] * M.data[3],
			data[1] * M.data[0] + data[3] * M.data[1], data[1] * M.data[2] + data[3] * M.data[3]);
	}

	vec2 mat2::operator*(const vec2& V) const
	{
		return vec2(
			data[RightX] * V.x + data[UpX] * V.y,
			data[RightY] * V.x + data[UpY] * V.y);
	}

	mat2 mat2::operator*(float scalar) const
	{
		return mat2(
			data[0] * scalar, data[2] * scalar,
			data[1] * scalar, data[3] * scalar);
	}

	mat2 mat2::operator/(float divisor) const
	{
		return *this * (1.0f / divisor);
	}

	mat2 mat2::operator-() const
	{
		return mat2(
			-data[0], -data[2],
			-data[1], -data[3]);
	}

	float mat2::operator[](unsigned index) const
	{
		ASSERT(index < 9, "'index' must be in the range of [0, 8].");
		return data[index];
	}

	float& mat2::operator[](unsigned index)
	{
		ASSERT(index < 9, "'index' must be in the range of [0, 8].");
		return data[index];
	}

	void mat2::Transpose()
	{
		std::swap(data[0], data[3]);
		std::swap(data[1], data[2]);
	}

	void mat2::Inverse()
	{
		float det = GetDeterminant();
		if (det == 0.0f)
			return;

		*this = mat2(
			data[3], -data[2],
			-data[1], data[0]) / det;
	}

	mat2 mat2::GetTranspose() const
	{
		mat2 result(*this);
		result.Transpose();
		return result;
	}

	mat2 mat2::GetInverse() const
	{
		mat2 result(*this);
		result.Inverse();
		return result;
	}

	float mat2::GetDeterminant() const
	{
		return data[0] * data[3] - data[1] * data[2];
	}

	void mat2::Scale(const vec2& scale)
	{
		*this = mat2(
			scale.x, 0.0f,
			0.0f, scale.y) * (*this);
	}

	void mat2::Scale(float scale)
	{
		*this = mat2(
			scale, 0.0f,
			0.0f, scale) * (*this);
	}

	void mat2::Rotate(float degrees)
	{
		float radians = ToRadian(degrees);
		float cosValue = cos(radians);
		float sinValue = sin(radians);

		*this = mat2(
			cosValue, -sinValue,
			sinValue, cosValue) * (*this);
	}

	void mat2::SetRight(const vec2& V)
	{
		data[RightX] = V.x;
		data[RightY] = V.y;
	}

	void mat2::SetUp(const vec2& V)
	{
		data[UpX] = V.x;
		data[UpY] = V.y;
	}

	vec2 mat2::GetRight() const
	{
		return vec2(data[RightX], data[RightY]);
	}

	vec2 mat2::GetUp() const
	{
		return vec2(data[UpX], data[UpY]);
	}

	const mat3 mat3::Identity = mat3();

	mat3::mat3()
	{
		data[0] = 1.0f;
		data[1] = 0.0f;
		data[2] = 0.0f;

		data[3] = 0.0f;
		data[4] = 1.0f;
		data[5] = 0.0f;

		data[6] = 0.0f;
		data[7] = 0.0f;
		data[8] = 1.0f;
	}

	mat3::mat3(const quat& rotation)
	{
		data[0] = 1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z);
		data[1] = 2.0f * (rotation.x * rotation.y + rotation.z * rotation.w);
		data[2] = 2.0f * (rotation.x * rotation.z - rotation.y * rotation.w);

		data[3] = 2.0f * (rotation.x * rotation.y - rotation.z * rotation.w);
		data[4] = 1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z);
		data[5] = 2.0f * (rotation.y * rotation.z + rotation.x * rotation.w);

		data[6] = 2.0f * (rotation.x * rotation.z + rotation.y * rotation.w);
		data[7] = 2.0f * (rotation.y * rotation.z - rotation.x * rotation.w);
		data[8] = 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y);
	}

	mat3::mat3(const mat4& mat)
	{
		data[RightX] = mat.data[mat4::RightX];
		data[RightY] = mat.data[mat4::RightY];
		data[RightZ] = mat.data[mat4::RightZ];

		data[UpX] = mat.data[mat4::UpX];
		data[UpY] = mat.data[mat4::UpY];
		data[UpZ] = mat.data[mat4::UpZ];

		data[ForwardX] = mat.data[mat4::ForwardX];
		data[ForwardY] = mat.data[mat4::ForwardY];
		data[ForwardZ] = mat.data[mat4::ForwardZ];
	}

	mat3::mat3(const quat& rotation, const vec3& scale)
		: mat3(rotation)
	{
		Scale(scale);
	}

	mat3::mat3(const vec3& right, const vec3& up, const vec3& forward)
	{
		data[RightX] = right.x;
		data[RightY] = right.y;
		data[RightZ] = right.z;

		data[UpX] = up.x;
		data[UpY] = up.y;
		data[UpZ] = up.z;

		data[ForwardX] = forward.x;
		data[ForwardY] = forward.y;
		data[ForwardZ] = forward.z;
	}

	mat3::mat3(float f0, float f3, float f6, float f1, float f4, float f7, float f2, float f5, float f8)
	{
		data[0] = f0;
		data[1] = f1;
		data[2] = f2;

		data[3] = f3;
		data[4] = f4;
		data[5] = f5;

		data[6] = f6;
		data[7] = f7;
		data[8] = f8;
	}

	bool mat3::operator==(const mat3& M) const
	{
		for (int i = 0; i < 9; ++i)
		{
			if (!Equals(data[i], M.data[i]))
				return false;
		}

		return true;
	}

	bool mat3::operator!=(const mat3& M) const
	{
		for (int i = 0; i < 9; ++i)
		{
			if (!Equals(data[i], M.data[i]))
				return true;
		}

		return false;
	}

	mat3& mat3::operator*=(const mat3& M)
	{
		return *this = (*this) * M;
	}

	mat3& mat3::operator*=(float scalar)
	{
		data[0] *= scalar;
		data[1] *= scalar;
		data[2] *= scalar;

		data[3] *= scalar;
		data[4] *= scalar;
		data[5] *= scalar;

		data[6] *= scalar;
		data[7] *= scalar;
		data[8] *= scalar;

		return *this;
	}

	mat3& mat3::operator/=(float divisor)
	{
		float inverse = 1.0f / divisor;

		data[0] *= inverse;
		data[1] *= inverse;
		data[2] *= inverse;

		data[3] *= inverse;
		data[4] *= inverse;
		data[5] *= inverse;

		data[6] *= inverse;
		data[7] *= inverse;
		data[8] *= inverse;

		return *this;
	}

	mat3& mat3::operator+=(const mat3& M)
	{
		data[0] += M.data[0];
		data[1] += M.data[1];
		data[2] += M.data[2];

		data[3] += M.data[3];
		data[4] += M.data[4];
		data[5] += M.data[5];

		data[6] += M.data[6];
		data[7] += M.data[7];
		data[8] += M.data[8];

		return *this;
	}

	mat3& mat3::operator-=(const mat3& M)
	{
		data[0] -= M.data[0];
		data[1] -= M.data[1];
		data[2] -= M.data[2];

		data[3] -= M.data[3];
		data[4] -= M.data[4];
		data[5] -= M.data[5];

		data[6] -= M.data[6];
		data[7] -= M.data[7];
		data[8] -= M.data[8];

		return *this;
	}

	mat3 mat3::operator*(const mat3& M) const
	{
		return mat3(
			data[0] * M.data[0] + data[3] * M.data[1] + data[6] * M.data[2],
			data[0] * M.data[3] + data[3] * M.data[4] + data[6] * M.data[5],
			data[0] * M.data[6] + data[3] * M.data[7] + data[6] * M.data[8],
			data[1] * M.data[0] + data[4] * M.data[1] + data[7] * M.data[2],
			data[1] * M.data[3] + data[4] * M.data[4] + data[7] * M.data[5],
			data[1] * M.data[6] + data[4] * M.data[7] + data[7] * M.data[8],
			data[2] * M.data[0] + data[5] * M.data[1] + data[8] * M.data[2],
			data[2] * M.data[3] + data[5] * M.data[4] + data[8] * M.data[5],
			data[2] * M.data[6] + data[5] * M.data[7] + data[8] * M.data[8]);
	}

	mat3 mat3::operator+(const mat3& M) const
	{
		return mat3(
			data[0] + M.data[0], data[3] + M.data[3], data[6] + M.data[6],
			data[1] + M.data[1], data[4] + M.data[4], data[7] + M.data[7],
			data[2] + M.data[2], data[5] + M.data[5], data[8] + M.data[8]);
	}

	mat3 mat3::operator-(const mat3& M) const
	{
		return mat3(
			data[0] - M.data[0], data[3] - M.data[3], data[6] - M.data[6],
			data[1] - M.data[1], data[4] - M.data[4], data[7] - M.data[7],
			data[2] - M.data[2], data[5] - M.data[5], data[8] - M.data[8]);
	}

	vec3 mat3::operator*(const vec3& V) const
	{
		return vec3(
			data[RightX] * V.x + data[UpX] * V.y + data[ForwardX] * V.z,
			data[RightY] * V.x + data[UpY] * V.y + data[ForwardY] * V.z,
			data[RightZ] * V.x + data[UpZ] * V.y + data[ForwardZ] * V.z);
	}

	mat3 mat3::operator*(float scalar) const
	{
		return mat3(
			data[0] * scalar, data[3] * scalar, data[6] * scalar,
			data[1] * scalar, data[4] * scalar, data[7] * scalar,
			data[2] * scalar, data[5] * scalar, data[8] * scalar);
	}

	mat3 mat3::operator/(float divisor) const
	{
		return *this * (1.0f / divisor);
	}

	mat3 mat3::operator-() const
	{
		return mat3(
			-data[0], -data[3], -data[6],
			-data[1], -data[4], -data[7],
			-data[2], -data[5], -data[8]);
	}

	float mat3::operator[](unsigned index) const
	{
		ASSERT(index < 9, "'index' must be in the range of [0, 8].");
		return data[index];
	}

	float& mat3::operator[](unsigned index)
	{
		ASSERT(index < 9, "'index' must be in the range of [0, 8].");
		return data[index];
	}

	void mat3::Transpose()
	{
		std::swap(data[1], data[3]);
		std::swap(data[2], data[6]);
		std::swap(data[5], data[7]);
	}

	void mat3::Inverse()
	{
		float det = GetDeterminant();
		if (det == 0.0f)
			return;

		*this = mat3(
			data[4] * data[8] - data[7] * data[5],
			data[6] * data[5] - data[3] * data[8],
			data[3] * data[7] - data[6] * data[6],
			data[7] * data[2] - data[1] * data[8],
			data[0] * data[8] - data[6] * data[2],
			data[6] * data[1] - data[0] * data[7],
			data[1] * data[5] - data[4] * data[2],
			data[3] * data[2] - data[0] * data[5],
			data[0] * data[4] - data[3] * data[1]) / det;
	}

	mat3 mat3::GetTranspose() const
	{
		mat3 result(*this);
		result.Transpose();
		return result;
	}

	mat3 mat3::GetInverse() const
	{
		mat3 result(*this);
		result.Inverse();
		return result;
	}

	float mat3::GetDeterminant() const
	{
		return
			data[0] * (data[4] * data[8] - data[7] * data[5]) -
			data[3] * (data[1] * data[8] - data[7] * data[2]) +
			data[6] * (data[1] * data[5] - data[4] * data[2]);
	}

	void mat3::Scale(const vec3& scale)
	{
		data[RightX] *= scale.x;
		data[RightY] *= scale.x;
		data[RightZ] *= scale.x;
		data[UpX] *= scale.y;
		data[UpY] *= scale.y;
		data[UpZ] *= scale.y;
		data[ForwardX] *= scale.z;
		data[ForwardY] *= scale.z;
		data[ForwardZ] *= scale.z;
	}

	void mat3::Scale(float scale)
	{
		Scale(vec3(scale));
	}

	void mat3::Rotate(const vec3& axis, float degrees)
	{
		float radians = ToRadian(degrees);
		float sinValue = sin(radians);
		float cosValue = cos(radians);
		float oneMinusCosValue = 1.0f - cosValue;

		*this = mat3(
			axis.x * axis.x + cosValue * (1.0f - axis.x * axis.x),
			axis.x * axis.y * oneMinusCosValue - axis.z * sinValue,
			axis.x * axis.z * oneMinusCosValue + axis.y * sinValue,
			axis.x * axis.y * oneMinusCosValue + axis.z * sinValue,
			axis.y * axis.y + cosValue * (1.0f - axis.y * axis.y),
			axis.y * axis.z * oneMinusCosValue - axis.x * sinValue,
			axis.x * axis.z * oneMinusCosValue - axis.y * sinValue,
			axis.y * axis.z * oneMinusCosValue + axis.x * sinValue,
			axis.z * axis.z + cosValue * (1.0f - axis.z * axis.z)) * (*this);
	}

	void mat3::RotateX(float degrees)
	{
		float radians = ToRadian(degrees);
		float cosValue = cos(radians);
		float sinValue = sin(radians);

		*this = mat3(
			1.0f, 0.0f, 0.0f,
			0.0f, cosValue, -sinValue,
			0.0f, sinValue, cosValue) * (*this);
	}

	void mat3::RotateY(float degrees)
	{
		float radians = ToRadian(degrees);
		float cosValue = cos(radians);
		float sinValue = sin(radians);

		*this = mat3(
			cosValue, 0.0f, sinValue,
			0.0f, 1.0f, 0.0f,
			-sinValue, 0.0f, cosValue) * (*this);
	}

	void mat3::RotateZ(float degrees)
	{
		float radians = ToRadian(degrees);
		float cosValue = cos(radians);
		float sinValue = sin(radians);

		*this = mat3(
			cosValue, -sinValue, 0.0f,
			sinValue, cosValue, 0.0f,
			0.0f, 0.0f, 1.0f) * (*this);
	}

	void mat3::SetRight(const vec3& V)
	{
		data[RightX] = V.x;
		data[RightY] = V.y;
		data[RightZ] = V.z;
	}

	void mat3::SetUp(const vec3& V)
	{
		data[UpX] = V.x;
		data[UpY] = V.y;
		data[UpZ] = V.z;
	}

	void mat3::SetForward(const vec3& V)
	{
		data[ForwardX] = V.x;
		data[ForwardY] = V.y;
		data[ForwardZ] = V.z;
	}

	vec3 mat3::GetRight() const
	{
		return vec3(data[RightX], data[RightY], data[RightZ]);
	}

	vec3 mat3::GetUp() const
	{
		return vec3(data[UpX], data[UpY], data[UpZ]);
	}

	vec3 mat3::GetForward() const
	{
		return vec3(data[ForwardX], data[ForwardY], data[ForwardZ]);
	}

	mat3 mat3::LookAt(const vec3& forward, const vec3& upAnchor)
	{
		const vec3 right = Normalize(Cross(forward, upAnchor));
		const vec3 up = Cross(right, forward);

		return mat3(right, up, -forward);
	}

	const mat4 mat4::Identity = mat4();

	mat4::mat4()
	{
		data[0] = 1.0f;
		data[1] = 0.0f;
		data[2] = 0.0f;
		data[3] = 0.0f;

		data[4] = 0.0f;
		data[5] = 1.0f;
		data[6] = 0.0f;
		data[7] = 0.0f;

		data[8] = 0.0f;
		data[9] = 0.0f;
		data[10] = 1.0f;
		data[11] = 0.0f;

		data[12] = 0.0f;
		data[13] = 0.0f;
		data[14] = 0.0f;
		data[15] = 1.0f;
	}

	mat4::mat4(const quat& rotation)
	{
		data[0] = 1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z);
		data[1] = 2.0f * (rotation.x * rotation.y + rotation.z * rotation.w);
		data[2] = 2.0f * (rotation.x * rotation.z - rotation.y * rotation.w);
		data[3] = 0.0f;

		data[4] = 2.0f * (rotation.x * rotation.y - rotation.z * rotation.w);
		data[5] = 1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z);
		data[6] = 2.0f * (rotation.y * rotation.z + rotation.x * rotation.w);
		data[7] = 0.0f;

		data[8] = 2.0f * (rotation.x * rotation.z + rotation.y * rotation.w);
		data[9] = 2.0f * (rotation.y * rotation.z - rotation.x * rotation.w);
		data[10] = 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y);
		data[11] = 0.0f;

		data[12] = 0.0f;
		data[13] = 0.0f;
		data[14] = 0.0f;
		data[15] = 1.0f;
	}

	mat4::mat4(const mat3& rotation)
	{
		data[RightX] = rotation.data[mat3::RightX];
		data[RightY] = rotation.data[mat3::RightY];
		data[RightZ] = rotation.data[mat3::RightZ];
		data[W0] = 0.0f;

		data[UpX] = rotation.data[mat3::UpX];
		data[UpY] = rotation.data[mat3::UpY];
		data[UpZ] = rotation.data[mat3::UpZ];
		data[W1] = 0.0f;

		data[ForwardX] = rotation.data[mat3::ForwardX];
		data[ForwardY] = rotation.data[mat3::ForwardY];
		data[ForwardZ] = rotation.data[mat3::ForwardZ];
		data[W2] = 0.0f;

		data[TransX] = 0.0f;
		data[TransY] = 0.0f;
		data[TransZ] = 0.0f;
		data[W3] = 1.0f;
	}

	mat4::mat4(const quat& rotation, const vec3& translation)
	{
		data[0] = 1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z);
		data[1] = 2.0f * (rotation.x * rotation.y + rotation.z * rotation.w);
		data[2] = 2.0f * (rotation.x * rotation.z - rotation.y * rotation.w);
		data[3] = 0.0f;

		data[4] = 2.0f * (rotation.x * rotation.y - rotation.z * rotation.w);
		data[5] = 1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z);
		data[6] = 2.0f * (rotation.y * rotation.z + rotation.x * rotation.w);
		data[7] = 0.0f;

		data[8] = 2.0f * (rotation.x * rotation.z + rotation.y * rotation.w);
		data[9] = 2.0f * (rotation.y * rotation.z - rotation.x * rotation.w);
		data[10] = 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y);
		data[11] = 0.0f;

		data[12] = translation.x;
		data[13] = translation.y;
		data[14] = translation.z;
		data[15] = 1.0f;
	}

	mat4::mat4(const mat3& rotation, const vec3& translation)
	{
		data[RightX] = rotation.data[mat3::RightX];
		data[RightY] = rotation.data[mat3::RightY];
		data[RightZ] = rotation.data[mat3::RightZ];
		data[W0] = 0.0f;

		data[UpX] = rotation.data[mat3::UpX];
		data[UpY] = rotation.data[mat3::UpY];
		data[UpZ] = rotation.data[mat3::UpZ];
		data[W1] = 0.0f;

		data[ForwardX] = rotation.data[mat3::ForwardX];
		data[ForwardY] = rotation.data[mat3::ForwardY];
		data[ForwardZ] = rotation.data[mat3::ForwardZ];
		data[W2] = 0.0f;

		data[TransX] = translation.x;
		data[TransY] = translation.y;
		data[TransZ] = translation.z;
		data[W3] = 1.0f;
	}

	mat4::mat4(const quat& rotation, const vec3& translation, const vec3& scale)
		: mat4(rotation, translation)
	{
		Scale(scale);
	}

	mat4::mat4(const mat3& rotation, const vec3& translation, const vec3& scale)
		: mat4(rotation, translation)
	{
		Scale(scale);
	}

	mat4::mat4(const vec3& right, const vec3& up, const vec3& forward, const vec3& translation)
	{
		data[RightX] = right.x;
		data[RightY] = right.y;
		data[RightZ] = right.z;
		data[W0] = 0.0f;

		data[UpX] = up.x;
		data[UpY] = up.y;
		data[UpZ] = up.z;
		data[W1] = 0.0f;

		data[ForwardX] = forward.x;
		data[ForwardY] = forward.y;
		data[ForwardZ] = forward.z;
		data[W2] = 0.0f;

		data[TransX] = translation.x;
		data[TransY] = translation.y;
		data[TransZ] = translation.z;
		data[W3] = 1.0f;
	}

	mat4::mat4(float f0, float f4, float f8, float f12, float f1, float f5, float f9, float f13, float f2, float f6, float f10, float f14, float f3, float f7, float f11, float f15)
	{
		data[0] = f0;
		data[1] = f1;
		data[2] = f2;
		data[3] = f3;

		data[4] = f4;
		data[5] = f5;
		data[6] = f6;
		data[7] = f7;

		data[8] = f8;
		data[9] = f9;
		data[10] = f10;
		data[11] = f11;

		data[12] = f12;
		data[13] = f13;
		data[14] = f14;
		data[15] = f15;
	}

	bool mat4::operator==(const mat4& M) const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (!Equals(data[i], M.data[i]))
				return false;
		}

		return true;
	}

	bool mat4::operator!=(const mat4& M) const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (!Equals(data[i], M.data[i]))
				return true;
		}

		return false;
	}

	mat4& mat4::operator*=(const mat4& M)
	{
		return *this = (*this) * M;
	}

	mat4& mat4::operator*=(float scalar)
	{
		data[0] *= scalar;
		data[1] *= scalar;
		data[2] *= scalar;
		data[3] *= scalar;

		data[4] *= scalar;
		data[5] *= scalar;
		data[6] *= scalar;
		data[7] *= scalar;

		data[8] *= scalar;
		data[9] *= scalar;
		data[10] *= scalar;
		data[11] *= scalar;

		data[12] *= scalar;
		data[13] *= scalar;
		data[14] *= scalar;
		data[15] *= scalar;

		return *this;
	}

	mat4& mat4::operator/=(float divisor)
	{
		float inverse = 1.0f / divisor;

		data[0] *= inverse;
		data[1] *= inverse;
		data[2] *= inverse;
		data[3] *= inverse;

		data[4] *= inverse;
		data[5] *= inverse;
		data[6] *= inverse;
		data[7] *= inverse;

		data[8] *= inverse;
		data[9] *= inverse;
		data[10] *= inverse;
		data[11] *= inverse;

		data[12] *= inverse;
		data[13] *= inverse;
		data[14] *= inverse;
		data[15] *= inverse;

		return *this;
	}

	mat4& mat4::operator+=(const mat4& M)
	{
		data[0] += M.data[0];
		data[1] += M.data[1];
		data[2] += M.data[2];
		data[3] += M.data[3];

		data[4] += M.data[4];
		data[5] += M.data[5];
		data[6] += M.data[6];
		data[7] += M.data[7];

		data[8] += M.data[8];
		data[9] += M.data[9];
		data[10] += M.data[10];
		data[11] += M.data[11];

		data[12] += M.data[12];
		data[13] += M.data[13];
		data[14] += M.data[14];
		data[15] += M.data[15];

		return *this;
	}

	mat4& mat4::operator-=(const mat4& M)
	{
		data[0] -= M.data[0];
		data[1] -= M.data[1];
		data[2] -= M.data[2];
		data[3] -= M.data[3];

		data[4] -= M.data[4];
		data[5] -= M.data[5];
		data[6] -= M.data[6];
		data[7] -= M.data[7];

		data[8] -= M.data[8];
		data[9] -= M.data[9];
		data[10] -= M.data[10];
		data[11] -= M.data[11];

		data[12] -= M.data[12];
		data[13] -= M.data[13];
		data[14] -= M.data[14];
		data[15] -= M.data[15];

		return *this;
	}

	mat4 mat4::operator*(const mat4& M) const
	{
		return mat4(
			M.data[0] * data[0] + M.data[1] * data[4] + M.data[2] * data[8] + M.data[3] * data[12],
			M.data[4] * data[0] + M.data[5] * data[4] + M.data[6] * data[8] + M.data[7] * data[12],
			M.data[8] * data[0] + M.data[9] * data[4] + M.data[10] * data[8] + M.data[11] * data[12],
			M.data[12] * data[0] + M.data[13] * data[4] + M.data[14] * data[8] + M.data[15] * data[12],
			M.data[0] * data[1] + M.data[1] * data[5] + M.data[2] * data[9] + M.data[3] * data[13],
			M.data[4] * data[1] + M.data[5] * data[5] + M.data[6] * data[9] + M.data[7] * data[13],
			M.data[8] * data[1] + M.data[9] * data[5] + M.data[10] * data[9] + M.data[11] * data[13],
			M.data[12] * data[1] + M.data[13] * data[5] + M.data[14] * data[9] + M.data[15] * data[13],
			M.data[0] * data[2] + M.data[1] * data[6] + M.data[2] * data[10] + M.data[3] * data[14],
			M.data[4] * data[2] + M.data[5] * data[6] + M.data[6] * data[10] + M.data[7] * data[14],
			M.data[8] * data[2] + M.data[9] * data[6] + M.data[10] * data[10] + M.data[11] * data[14],
			M.data[12] * data[2] + M.data[13] * data[6] + M.data[14] * data[10] + M.data[15] * data[14],
			M.data[0] * data[3] + M.data[1] * data[7] + M.data[2] * data[11] + M.data[3] * data[15],
			M.data[4] * data[3] + M.data[5] * data[7] + M.data[6] * data[11] + M.data[7] * data[15],
			M.data[8] * data[3] + M.data[9] * data[7] + M.data[10] * data[11] + M.data[11] * data[15],
			M.data[12] * data[3] + M.data[13] * data[7] + M.data[14] * data[11] + M.data[15] * data[15]);
	}

	mat4 mat4::operator+(const mat4& M) const
	{
		return mat4(
			data[0] + M.data[0], data[4] + M.data[4], data[8] + M.data[8], data[12] + M.data[12],
			data[1] + M.data[1], data[5] + M.data[5], data[9] + M.data[9], data[13] + M.data[13],
			data[2] + M.data[2], data[6] + M.data[6], data[10] + M.data[10], data[14] + M.data[14],
			data[3] + M.data[3], data[7] + M.data[7], data[11] + M.data[11], data[15] + M.data[15]);
	}

	mat4 mat4::operator-(const mat4& M) const
	{
		return mat4(
			data[0] - M.data[0], data[4] - M.data[4], data[8] - M.data[8], data[12] - M.data[12],
			data[1] - M.data[1], data[5] - M.data[5], data[9] - M.data[9], data[13] - M.data[13],
			data[2] - M.data[2], data[6] - M.data[6], data[10] - M.data[10], data[14] - M.data[14],
			data[3] - M.data[3], data[7] - M.data[7], data[11] - M.data[11], data[15] - M.data[15]);
	}

	vec4 mat4::operator*(const vec4& V) const
	{
		return vec4(
			data[0] * V.x + data[4] * V.y + data[8] * V.z + data[12] * V.w,
			data[1] * V.x + data[5] * V.y + data[9] * V.z + data[13] * V.w,
			data[2] * V.x + data[6] * V.y + data[10] * V.z + data[14] * V.w,
			data[3] * V.x + data[7] * V.y + data[11] * V.z + data[15] * V.w);
	}

	mat4 mat4::operator*(float scalar) const
	{
		return mat4(
			data[0] * scalar, data[4] * scalar, data[8] * scalar, data[12] * scalar,
			data[1] * scalar, data[5] * scalar, data[9] * scalar, data[13] * scalar,
			data[2] * scalar, data[6] * scalar, data[10] * scalar, data[14] * scalar,
			data[3] * scalar, data[7] * scalar, data[11] * scalar, data[15] * scalar);
	};

	mat4 mat4::operator/(float divisor) const
	{
		return *this * (1.0f / divisor);
	}

	mat4 mat4::operator-() const
	{
		return mat4(
			-data[0], -data[4], -data[8], -data[12],
			-data[1], -data[5], -data[9], -data[13],
			-data[2], -data[6], -data[10], -data[14],
			-data[3], -data[7], -data[11], -data[15]);
	}

	float mat4::operator[](unsigned index) const
	{
		ASSERT(index < 16, "'index' must be in the range of [0, 15].");
		return data[index];
	}

	float& mat4::operator[](unsigned index)
	{
		ASSERT(index < 16, "'index' must be in the range of [0, 15].");
		return data[index];
	}

	void mat4::Transpose()
	{
		std::swap(data[1], data[4]);
		std::swap(data[2], data[8]);
		std::swap(data[3], data[12]);
		std::swap(data[6], data[9]);
		std::swap(data[7], data[13]);
		std::swap(data[11], data[14]);
	}

	void mat4::Inverse()
	{
		mat4 inv;

		inv[0] = data[5] * data[10] * data[15] -
			data[5]  * data[11] * data[14] -
			data[9]  * data[6]  * data[15] +
			data[9]  * data[7]  * data[14] +
			data[13] * data[6]  * data[11] -
			data[13] * data[7]  * data[10];

		inv[4] = -data[4] * data[10] * data[15] +
			data[4]  * data[11] * data[14] +
			data[8]  * data[6]  * data[15] -
			data[8]  * data[7]  * data[14] -
			data[12] * data[6]  * data[11] +
			data[12] * data[7]  * data[10];

		inv[8] = data[4] * data[9] * data[15] -
			data[4]  * data[11] * data[13] -
			data[8]  * data[5] * data[15] +
			data[8]  * data[7] * data[13] +
			data[12] * data[5] * data[11] -
			data[12] * data[7] * data[9];

		inv[12] = -data[4] * data[9] * data[14] +
			data[4]  * data[10] * data[13] +
			data[8]  * data[5] * data[14] -
			data[8]  * data[6] * data[13] -
			data[12] * data[5] * data[10] +
			data[12] * data[6] * data[9];

		float det = data[0] * inv[0] + data[1] * inv[4] + data[2] * inv[8] + data[3] * inv[12];
		if (det == 0.0f)
			return;

		inv[1] = -data[1] * data[10] * data[15] +
			data[1]  * data[11] * data[14] +
			data[9]  * data[2] * data[15] -
			data[9]  * data[3] * data[14] -
			data[13] * data[2] * data[11] +
			data[13] * data[3] * data[10];

		inv[5] = data[0] * data[10] * data[15] -
			data[0]  * data[11] * data[14] -
			data[8]  * data[2] * data[15] +
			data[8]  * data[3] * data[14] +
			data[12] * data[2] * data[11] -
			data[12] * data[3] * data[10];

		inv[9] = -data[0] * data[9] * data[15] +
			data[0]  * data[11] * data[13] +
			data[8]  * data[1] * data[15] -
			data[8]  * data[3] * data[13] -
			data[12] * data[1] * data[11] +
			data[12] * data[3] * data[9];

		inv[13] = data[0] * data[9] * data[14] -
			data[0]  * data[10] * data[13] -
			data[8]  * data[1] * data[14] +
			data[8]  * data[2] * data[13] +
			data[12] * data[1] * data[10] -
			data[12] * data[2] * data[9];

		inv[2] = data[1] * data[6] * data[15] -
			data[1]  * data[7] * data[14] -
			data[5]  * data[2] * data[15] +
			data[5]  * data[3] * data[14] +
			data[13] * data[2] * data[7] -
			data[13] * data[3] * data[6];

		inv[6] = -data[0] * data[6] * data[15] +
			data[0]  * data[7] * data[14] +
			data[4]  * data[2] * data[15] -
			data[4]  * data[3] * data[14] -
			data[12] * data[2] * data[7] +
			data[12] * data[3] * data[6];

		inv[10] = data[0] * data[5] * data[15] -
			data[0]  * data[7] * data[13] -
			data[4]  * data[1] * data[15] +
			data[4]  * data[3] * data[13] +
			data[12] * data[1] * data[7] -
			data[12] * data[3] * data[5];

		inv[14] = -data[0] * data[5] * data[14] +
			data[0]  * data[6] * data[13] +
			data[4]  * data[1] * data[14] -
			data[4]  * data[2] * data[13] -
			data[12] * data[1] * data[6] +
			data[12] * data[2] * data[5];

		inv[3] = -data[1] * data[6] * data[11] +
			data[1] * data[7] * data[10] +
			data[5] * data[2] * data[11] -
			data[5] * data[3] * data[10] -
			data[9] * data[2] * data[7] +
			data[9] * data[3] * data[6];

		inv[7] = data[0] * data[6] * data[11] -
			data[0] * data[7] * data[10] -
			data[4] * data[2] * data[11] +
			data[4] * data[3] * data[10] +
			data[8] * data[2] * data[7] -
			data[8] * data[3] * data[6];

		inv[11] = -data[0] * data[5] * data[11] +
			data[0] * data[7] * data[9] +
			data[4] * data[1] * data[11] -
			data[4] * data[3] * data[9] -
			data[8] * data[1] * data[7] +
			data[8] * data[3] * data[5];

		inv[15] = data[0] * data[5] * data[10] -
			data[0] * data[6] * data[9] -
			data[4] * data[1] * data[10] +
			data[4] * data[2] * data[9] +
			data[8] * data[1] * data[6] -
			data[8] * data[2] * data[5];

		*this = inv / det;
	}

	void mat4::FastInverse()
	{
		mat3 rotation(*this);
		vec3 translation(this->GetTranslation());

		// Fast inverse of affine matrix.
		rotation.Transpose();

		translation = -rotation * translation;

		*this = mat4(rotation, translation);
	}

	mat4 mat4::GetTranspose() const
	{
		mat4 result(*this);
		result.Transpose();
		return result;
	}

	mat4 mat4::GetInverse() const
	{
		mat4 result(*this);
		result.Inverse();
		return result;
	}

	mat4 mat4::GetFastInverse() const
	{
		mat4 result(*this);
		result.FastInverse();
		return result;
	}

	void mat4::Scale(const vec3& scale)
	{
		data[RightX] *= scale.x;
		data[RightY] *= scale.x;
		data[RightZ] *= scale.x;
		data[UpX] *= scale.y;
		data[UpY] *= scale.y;
		data[UpZ] *= scale.y;
		data[ForwardX] *= scale.z;
		data[ForwardY] *= scale.z;
		data[ForwardZ] *= scale.z;
	}

	void mat4::Scale(float scale)
	{
		Scale(vec3(scale));
	}

	void mat4::Rotate(const vec3& axis, float degrees)
	{
		float radians = ToRadian(degrees);
		float sinValue = sin(radians);
		float cosValue = cos(radians);
		float oneMinusCosValue = 1.0f - cosValue;

		*this = mat4(
			axis.x * axis.x + cosValue * (1.0f - axis.x * axis.x),
			axis.x * axis.y * oneMinusCosValue - axis.z * sinValue,
			axis.x * axis.z * oneMinusCosValue + axis.y * sinValue, 0.0f,
			axis.x * axis.y * oneMinusCosValue + axis.z * sinValue,
			axis.y * axis.y + cosValue * (1.0f - axis.y * axis.y),
			axis.y * axis.z * oneMinusCosValue - axis.x * sinValue, 0.0f,
			axis.x * axis.z * oneMinusCosValue - axis.y * sinValue,
			axis.y * axis.z * oneMinusCosValue + axis.x * sinValue,
			axis.z * axis.z + cosValue * (1.0f - axis.z * axis.z), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f) * (*this);
	}

	void mat4::RotateX(float degrees)
	{
		float radians = ToRadian(degrees);
		float cosValue = cos(radians);
		float sinValue = sin(radians);

		*this = mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, cosValue, -sinValue, 0.0f,
			0.0f, sinValue, cosValue, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f) * (*this);
	}

	void mat4::RotateY(float degrees)
	{
		float radians = ToRadian(degrees);
		float cosValue = cos(radians);
		float sinValue = sin(radians);

		*this = mat4(
			cosValue, 0.0f, sinValue, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-sinValue, 0.0f, cosValue, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f) * (*this);
	}

	void mat4::RotateZ(float degrees)
	{
		float radians = ToRadian(degrees);
		float cosValue = cos(radians);
		float sinValue = sin(radians);

		*this = mat4(
			cosValue, -sinValue, 0.0f, 0.0f,
			sinValue, cosValue, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f) * (*this);
	}

	void mat4::SetRight(const vec3& V)
	{
		data[RightX] = V.x;
		data[RightY] = V.y;
		data[RightZ] = V.z;
	}

	void mat4::SetUp(const vec3& V)
	{
		data[UpX] = V.x;
		data[UpY] = V.y;
		data[UpZ] = V.z;
	}

	void mat4::SetForward(const vec3& V)
	{
		data[ForwardX] = V.x;
		data[ForwardY] = V.y;
		data[ForwardZ] = V.z;
	}

	void mat4::SetTranslation(const vec3& V)
	{
		data[TransX] = V.x;
		data[TransY] = V.y;
		data[TransZ] = V.z;
	}

	vec3 mat4::GetRight() const
	{
		return vec3(data[RightX], data[RightY], data[RightZ]);
	}

	vec3 mat4::GetUp() const
	{
		return vec3(data[UpX], data[UpY], data[UpZ]);
	}

	vec3 mat4::GetForward() const
	{
		return vec3(data[ForwardX], data[ForwardY], data[ForwardZ]);
	}

	vec3 mat4::GetTranslation() const
	{
		return vec3(data[TransX], data[TransY], data[TransZ]);
	}

	void mat4::Translate(const vec3& translation)
	{
		*this = mat4(
			1.0f, 0.0f, 0.0f, translation.x,
			0.0f, 1.0f, 0.0f, translation.y,
			0.0f, 0.0f, 1.0f, translation.z,
			0.0f, 0.0f, 0.0f, 1.0f) * (*this);
	}

	mat4 mat4::PerspectiveProjection(float fovyDegrees, float aspect, float zNear, float zFar)
	{
		mat4 result;
		float temp = 1.0f / tan(ToRadian(fovyDegrees) * 0.5f);

		result.data[0] = temp / aspect;
		result.data[1] = 0.0f;
		result.data[2] = 0.0f;
		result.data[3] = 0.0f;

		result.data[4] = 0.0f;
		result.data[5] = temp;
		result.data[6] = 0.0f;
		result.data[7] = 0.0f;

		result.data[8] = 0.0f;
		result.data[9] = 0.0f;
		result.data[10] = (zFar + zNear) / (zNear - zFar);
		result.data[11] = -1.0f;

		result.data[12] = 0.0f;
		result.data[13] = 0.0f;
		result.data[14] = (2.0f * zFar * zNear) / (zNear - zFar);
		result.data[15] = 0.0f;

		return result;
	}

	mat4 mat4::InversePerspectiveProjection(float fovyDegrees, float aspect, float zNear, float zFar)
	{
		mat4 result;
		float temp = 1.0f / tan(ToRadian(fovyDegrees) * 0.5f);

		result.data[0] = aspect / temp;
		result.data[1] = 0.0f;
		result.data[2] = 0.0f;
		result.data[3] = 0.0f;

		result.data[4] = 0.0f;
		result.data[5] = 1.0f / temp;
		result.data[6] = 0.0f;
		result.data[7] = 0.0f;

		result.data[8] = 0.0f;
		result.data[9] = 0.0f;
		result.data[10] = 0.0f;
		result.data[11] = (zNear - zFar) / (2.0f * zFar * zNear);

		result.data[12] = 0.0f;
		result.data[13] = 0.0f;
		result.data[14] = -1.0f;
		result.data[15] = 0.5f;

		return result;
	}

	mat4 mat4::OrthographicProjection(float left, float right, float top, float bottom, float zNear, float zFar)
	{
		mat4 result;

		result.data[0] = 2.0f / (right - left);
		result.data[1] = 0.0f;
		result.data[2] = 0.0f;
		result.data[3] = 0.0f;

		result.data[4] = 0.0f;
		result.data[5] = 2.0f / (top - bottom);
		result.data[6] = 0.0f;
		result.data[7] = 0.0f;

		result.data[8] = 0.0f;
		result.data[9] = 0.0f;
		result.data[10] = -2.0f / (zFar - zNear);
		result.data[11] = 0.0f;

		result.data[12] = -((right + left) / (right - left));
		result.data[13] = -((top + bottom) / (top - bottom));
		result.data[14] = -((zFar + zNear) / (zFar - zNear));
		result.data[15] = 1.0f;

		return result;
	}

	mat4 mat4::InverseOrthographicProjection(float left, float right, float top, float bottom, float zNear, float zFar)
	{
		mat4 result;

		result.data[0] = (right - left) / 2.0f;
		result.data[1] = 0.0f;
		result.data[2] = 0.0f;
		result.data[3] = 0.0f;

		result.data[4] = 0.0f;
		result.data[5] = (top - bottom) / 2.0f;
		result.data[6] = 0.0f;
		result.data[7] = 0.0f;

		result.data[8] = 0.0f;
		result.data[9] = 0.0f;
		result.data[10] = (zFar - zNear) / -2.0f;
		result.data[11] = 0.0f;

		result.data[12] = (right + left) / 2.0f;
		result.data[13] = (top + bottom) / 2.0f;
		result.data[14] = (zFar + zNear) / -2.0f;
		result.data[15] = 1.0f;

		return result;
	}

	mat4 mat4::LookAt(const vec3& position, const vec3& target, const vec3& upAnchor)
	{
		const vec3 forward = Normalize(target - position);
		const vec3 right = Normalize(Cross(forward, upAnchor));
		const vec3 up = Cross(right, forward);

		return mat4(right, up, -forward, position);
	}
}
