// Copyright (c) 2017 Emilian Cioca
#include "Quaternion.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Math.h"
#include "gemcutter/Math/Matrix.h"
#include "gemcutter/Math/Vector.h"

#include <loupe/loupe.h>

namespace gem
{
	const quat quat::Identity = quat();

	quat::quat(float X, float Y, float Z, float W)
		: x(X), y(Y), z(Z), w(W)
	{
	}

	quat::quat(const vec3& right, const vec3& up, const vec3& forward)
		: quat(mat3(right, up, forward))
	{
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

	void quat::FromEuler(const vec3& degrees)
	{
		float rx = ToRadian(degrees.x) * 0.5f;
		float ry = ToRadian(degrees.y) * 0.5f;
		float rz = ToRadian(degrees.z) * 0.5f;

	    float cx = cos(rx);
	    float cy = cos(ry);
		float cz = cos(rz);
	    float sx = sin(rx);
	    float sy = sin(ry);
	    float sz = sin(rz);

	    x = sx * cy * cz - cx * sy * sz;
	    y = cx * sy * cz + sx * cy * sz;
	    z = cx * cy * sz - sx * sy * cz;
	    w = cx * cy * cz + sx * sy * sz;
	}

	vec3 quat::GetEuler() const
	{
		vec3 angles;

	    float sinr_cosp = 2.0f * (w * x + y * z);
	    float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
	    angles.x = std::atan2(sinr_cosp, cosr_cosp);

	    float sinp = 2.0f * (w * y - z * x);
	    if (std::abs(sinp) >= 1.0f)
	    {
	        // Out of range.
	        angles.y = std::copysign(M_PI * 0.5f, sinp);
		}
	    else
	    {
	        angles.y = std::asin(sinp);
		}

	    float siny_cosp = 2.0f * (w * z + x * y);
	    float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
	    angles.z = std::atan2(siny_cosp, cosy_cosp);

		angles.x = ToDegree(angles.x);
		angles.y = ToDegree(angles.y);
		angles.z = ToDegree(angles.z);
		return angles;
	}

	void quat::Conjugate()
	{
		x *= -1.0f;
		y *= -1.0f;
		z *= -1.0f;
	}

	quat quat::GetConjugate() const
	{
		return { -x, -y, -z, w };
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

		return { x * invLength, y * invLength, z * invLength, w * invLength };
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
		if (Equals(dot, 1.0f))
		{
			// Quaternions are the same.
			return p1;
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

REFLECT(gem::quat)
	MEMBERS {
		REF_MEMBER(x)
		REF_MEMBER(y)
		REF_MEMBER(z)
		REF_MEMBER(w)
	}
REF_END;
