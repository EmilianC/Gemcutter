// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Quaternion.h"
#include "Math.h"
#include "Matrix.h"
#include "Vector.h"
#include "Jewel3D/Application/Logging.h"

namespace Jwl
{
	const quat quat::Identity = quat();

	quat::quat(float X, float Y, float Z, float W)
		: x(X), y(Y), z(Z), w(W)
	{
	}

	quat::quat(const vec3& right, const vec3& up, const vec3& forward)
	{
		*this = quat(mat3(right, up, forward));
	}

	quat::quat(const mat3& rotation)
	{
		float trace = rotation.data[0] + rotation.data[4] + rotation.data[8];

		if (trace > 0.0f)
		{
			float S = sqrtf(trace + 1.0f) * 2.0f;
			x = (rotation.data[5] - rotation.data[7]) / S;
			y = (rotation.data[6] - rotation.data[2]) / S;
			z = (rotation.data[1] - rotation.data[3]) / S;
			w = 0.25f * S;
		}
		else if ((rotation.data[0] > rotation.data[4]) && (rotation.data[0] > rotation.data[8]))
		{
			float S = sqrtf(1.0f + rotation.data[0] - rotation.data[4] - rotation.data[8]) * 2.0f;
			x = 0.25f * S;
			y = (rotation.data[3] + rotation.data[1]) / S;
			z = (rotation.data[6] + rotation.data[2]) / S;
			w = (rotation.data[5] - rotation.data[7]) / S;
		}
		else if (rotation.data[4] > rotation.data[8])
		{
			float S = sqrtf(1.0f + rotation.data[4] - rotation.data[0] - rotation.data[8]) * 2.0f;
			x = (rotation.data[3] + rotation.data[1]) / S;
			y = 0.25f * S;
			z = (rotation.data[7] + rotation.data[5]) / S;
			w = (rotation.data[6] - rotation.data[2]) / S;
		}
		else
		{
			float S = sqrtf(1.0f + rotation.data[8] - rotation.data[0] - rotation.data[4]) * 2.0f;
			x = (rotation.data[6] + rotation.data[2]) / S;
			y = (rotation.data[7] + rotation.data[5]) / S;
			z = 0.25f * S;
			w = (rotation.data[1] - rotation.data[3]) / S;
		}

		Normalize();
	}

	bool quat::operator==(const quat& other) const
	{
		return
			Equals(x, other.x) &&
			Equals(y, other.y) &&
			Equals(z, other.z) &&
			Equals(w, other.w);
	}

	bool quat::operator!=(const quat& other) const
	{
		return
			!Equals(x, other.x) ||
			!Equals(y, other.y) ||
			!Equals(z, other.z) ||
			!Equals(w, other.w);
	}

	quat quat::operator*(const quat& other) const
	{
		return quat(
			w * other.x + x * other.w + y * other.z - z * other.y,
			w * other.y - x * other.z + y * other.w + z * other.x,
			w * other.z + x * other.y - y * other.x + z * other.w,
			w * other.w - x * other.x - y * other.y - z * other.z);
	}

	vec3 quat::operator*(const vec3& v) const
	{
		vec3 q = vec3(x, y, z);

		return 2.0f * Dot(q, v) * q
			+ (w * w - Dot(q, q)) * v
			+ 2.0f * w * Cross(q, v);
	}

	quat& quat::operator*=(const quat& other)
	{
		*this = (*this) * other;

		return *this;
	}

	float quat::operator[](unsigned index) const
	{
		return *(&x + index);
	}

	float& quat::operator[](unsigned index)
	{
		return *(&x + index);
	}

	void quat::SetIdentity()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	void quat::Conjugate()
	{
		x *= -1.0f;
		y *= -1.0f;
		z *= -1.0f;
	}

	quat quat::GetConjugate() const
	{
		return quat(-x, -y, -z, w);
	}

	void quat::Normalize()
	{
		float invLength = 1.0f / sqrt(x * x + y * y + z * z + w * w);

		ASSERT(!std::isinf(invLength), "Zero length quaternion cannot be normalized.");

		x *= invLength;
		y *= invLength;
		z *= invLength;
		w *= invLength;
	}

	quat quat::GetNormalized() const
	{
		float invLength = 1.0f / sqrt(x * x + y * y + z * z + w * w);

		ASSERT(!std::isinf(invLength), "Zero length quaternion cannot be normalized.");

		return quat(x * invLength, y * invLength, z * invLength, w * invLength);
	}

	vec3 quat::GetRight() const
	{
		return *this * vec3::Right;
	}

	vec3 quat::GetUp() const
	{
		return *this * vec3::Up;
	}

	vec3 quat::GetForward() const
	{
		return *this * vec3::Forward;
	}

	void quat::Rotate(const vec3& axis, float degrees)
	{
		float radians = ToRadian(degrees) / 2.0f;
		vec3 temp = axis * sin(radians);

		*this = quat(temp.x, temp.y, temp.z, cos(radians)) * (*this);
	}

	void quat::Rotate(float X, float Y, float Z, float degrees)
	{
		float radians = ToRadian(degrees) / 2.0f;

		float sRadians = sin(radians);
		*this = quat(X * sRadians, Y * sRadians, Z * sRadians, cos(radians)) * (*this);
	}

	void quat::RotateX(float degrees)
	{
		float radians = ToRadian(degrees) / 2.0f;

		*this = quat(sin(radians), 0.0f, 0.0f, cos(radians)) * (*this);
	}

	void quat::RotateY(float degrees)
	{
		float radians = ToRadian(degrees) / 2.0f;

		*this = quat(0.0f, sin(radians), 0.0f, cos(radians)) * (*this);
	}

	void quat::RotateZ(float degrees)
	{
		float radians = ToRadian(degrees) / 2.0f;

		*this = quat(0.0f, 0.0f, sin(radians), cos(radians)) * (*this);
	}

	float Dot(const quat& p0, const quat& p1)
	{
		return p0.x * p1.x + p0.y * p1.y + p0.z * p1.z + p0.w * p1.w;
	}

	quat Slerp(const quat& p0, const quat& p1, float percent)
	{
		float dot = Abs(Dot(p0, p1));
		if (dot == 1.0f)
		{
			// Quaternions are the same.
			return p0;
		}

		float angle = acos(dot);
		float sAngle = sin(angle);
		float coef1 = sin((1.0f - percent) * angle) / sAngle;
		float coef2 = sin(percent * angle) / sAngle;

		return quat(
			coef1 * p0.x + coef2 * p1.x,
			coef1 * p0.y + coef2 * p1.y,
			coef1 * p0.z + coef2 * p1.z,
			coef1 * p0.w + coef2 * p1.w).GetNormalized();
	}
}
