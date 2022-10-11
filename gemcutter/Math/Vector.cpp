// Copyright (c) 2017 Emilian Cioca
#include "Vector.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Application/Reflection.h"
#include "gemcutter/Math/Math.h"

namespace gem
{
	const vec2 vec2::Zero  = vec2(0.0f, 0.0f);
	const vec2 vec2::One   = vec2(1.0f, 1.0f);
	const vec2 vec2::Left  = vec2(-1.0f, 0.0f);
	const vec2 vec2::Right = vec2(1.0f, 0.0f);
	const vec2 vec2::Up    = vec2(0.0f, 1.0f);
	const vec2 vec2::Down  = vec2(0.0f, -1.0f);

	vec2::vec2(float x, float y)
		: x(x), y(y)
	{
	}

	vec2::vec2(float val)
		: x(val), y(val)
	{
	}

	bool vec2::operator==(const vec2& RHS) const
	{
		return
			Equals(x, RHS.x) &&
			Equals(y, RHS.y);
	}

	bool vec2::operator!=(const vec2& RHS) const
	{
		return
			!Equals(x, RHS.x) ||
			!Equals(y, RHS.y);
	}

	vec2& vec2::operator-=(const vec2& RHS)
	{
		this->x -= RHS.x;
		this->y -= RHS.y;
		return *this;
	}

	vec2& vec2::operator+=(const vec2& RHS)
	{
		this->x += RHS.x;
		this->y += RHS.y;
		return *this;
	}

	vec2& vec2::operator*=(const vec2& RHS)
	{
		this->x *= RHS.x;
		this->y *= RHS.y;
		return *this;
	}

	vec2& vec2::operator/=(const vec2& RHS)
	{
		this->x /= RHS.x;
		this->y /= RHS.y;
		return *this;
	}

	vec2& vec2::operator*=(float scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		return *this;
	}

	vec2& vec2::operator/=(float divisor)
	{
		float inverse = 1.0f / divisor;
		this->x *= inverse;
		this->y *= inverse;
		return *this;
	}

	vec2 vec2::operator-() const
	{
		return { -x, -y };
	}

	vec2 vec2::operator-(const vec2& RHS) const
	{
		return { x - RHS.x, y - RHS.y };
	}

	vec2 vec2::operator+(const vec2& RHS) const
	{
		return { x + RHS.x, y + RHS.y };
	}

	vec2 vec2::operator*(const vec2& RHS) const
	{
		return { x * RHS.x, y * RHS.y };
	}

	vec2 vec2::operator/(const vec2& RHS) const
	{
		return { x / RHS.x, y / RHS.y };
	}

	vec2 vec2::operator*(float scalar) const
	{
		return { x * scalar, y * scalar };
	}

	vec2 vec2::operator/(float divisor) const
	{
		float inverse = 1.0f / divisor;
		return { x * inverse, y * inverse };
	}

	float vec2::operator[](unsigned index) const
	{
		ASSERT(index < 2, "'index' must be in the range of [0, 1].");
		return *(&x + index);
	}

	float& vec2::operator[](unsigned index)
	{
		ASSERT(index < 2, "'index' must be in the range of [0, 1].");
		return *(&x + index);
	}

	const vec3 vec3::Zero     = vec3(0.0f, 0.0f, 0.0f);
	const vec3 vec3::One      = vec3(1.0f, 1.0f, 1.0f);
	const vec3 vec3::Left     = vec3(-1.0f, 0.0f, 0.0f);
	const vec3 vec3::Right    = vec3(1.0f, 0.0f, 0.0f);
	const vec3 vec3::Up       = vec3(0.0f, 1.0f, 0.0f);
	const vec3 vec3::Down     = vec3(0.0f, -1.0f, 0.0f);
	const vec3 vec3::Forward  = vec3(0.0f, 0.0f, -1.0f);
	const vec3 vec3::Backward = vec3(0.0f, 0.0f, 1.0f);

	vec3::vec3(const vec2& xy, float z)
		: x(xy[0]), y(xy[1]), z(z)
	{
	}

	vec3::vec3(float x, float y, float z)
		: x(x), y(y), z(z)
	{
	}

	vec3::vec3(float val)
		: x(val), y(val), z(val)
	{
	}

	bool vec3::operator==(const vec3& RHS) const
	{
		return
			Equals(x, RHS.x) &&
			Equals(y, RHS.y) &&
			Equals(z, RHS.z);
	}

	bool vec3::operator!=(const vec3& RHS) const
	{
		return
			!Equals(x, RHS.x) ||
			!Equals(y, RHS.y) ||
			!Equals(z, RHS.z);
	}

	vec3& vec3::operator-=(const vec3& RHS)
	{
		this->x -= RHS.x;
		this->y -= RHS.y;
		this->z -= RHS.z;
		return *this;
	}

	vec3& vec3::operator+=(const vec3& RHS)
	{
		this->x += RHS.x;
		this->y += RHS.y;
		this->z += RHS.z;
		return *this;
	}

	vec3& vec3::operator*=(const vec3& RHS)
	{
		this->x *= RHS.x;
		this->y *= RHS.y;
		this->z *= RHS.z;
		return *this;
	}

	vec3& vec3::operator/=(const vec3& RHS)
	{
		this->x /= RHS.x;
		this->y /= RHS.y;
		this->z /= RHS.z;
		return *this;
	}

	vec3& vec3::operator*=(float scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
		return *this;
	}

	vec3& vec3::operator/=(float divisor)
	{
		float inverse = 1.0f / divisor;
		this->x *= inverse;
		this->y *= inverse;
		this->z *= inverse;
		return *this;
	}

	vec3 vec3::operator-() const
	{
		return { -x, -y, -z };
	}

	vec3 vec3::operator-(const vec3& RHS) const
	{
		return { x - RHS.x, y - RHS.y, z - RHS.z };
	}

	vec3 vec3::operator+(const vec3& RHS) const
	{
		return { x + RHS.x, y + RHS.y, z + RHS.z };
	}

	vec3 vec3::operator*(const vec3& RHS) const
	{
		return { x * RHS.x, y * RHS.y, z * RHS.z };
	}

	vec3 vec3::operator/(const vec3& RHS) const
	{
		return { x / RHS.x, y / RHS.y, z / RHS.z };
	}

	vec3 vec3::operator*(float scalar) const
	{
		return { x * scalar, y * scalar, z * scalar };
	}

	vec3 vec3::operator/(float divisor) const
	{
		float inverse = 1.0f / divisor;
		return { x * inverse, y * inverse, z * inverse };
	}

	float vec3::operator[](unsigned index) const
	{
		ASSERT(index < 3, "'index' must be in the range of [0, 2].");
		return *(&x + index);
	}

	float& vec3::operator[](unsigned index)
	{
		ASSERT(index < 3, "'index' must be in the range of [0, 2].");
		return *(&x + index);
	}

	vec3::operator vec2() const
	{
		return { x, y };
	}

	const vec4 vec4::Zero     = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	const vec4 vec4::One      = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	const vec4 vec4::Left     = vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	const vec4 vec4::Right    = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	const vec4 vec4::Up       = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	const vec4 vec4::Down     = vec4(0.0f, -1.0f, 0.0f, 0.0f);
	const vec4 vec4::Forward  = vec4(0.0f, 0.0f, -1.0f, 0.0f);
	const vec4 vec4::Backward = vec4(0.0f, 0.0f, 1.0f, 0.0f);

	vec4::vec4(const vec2& xy, float z, float w)
		: x(xy[0]), y(xy[1]), z(z), w(w)
	{
	}

	vec4::vec4(const vec2& xy, const vec2& zw)
		: x(xy[0]), y(xy[1]), z(zw[0]), w(zw[1])
	{
	}

	vec4::vec4(const vec3& xyz, float w)
		: x(xyz[0]), y(xyz[1]), z(xyz[2]), w(w)
	{
	}

	vec4::vec4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{
	}

	vec4::vec4(float val)
		: x(val), y(val), z(val), w(val)
	{
	}

	bool vec4::operator==(const vec4& RHS) const
	{
		return
			Equals(x, RHS.x) &&
			Equals(y, RHS.y) &&
			Equals(z, RHS.z) &&
			Equals(w, RHS.w);
	}

	bool vec4::operator!=(const vec4& RHS) const
	{
		return
			!Equals(x, RHS.x) ||
			!Equals(y, RHS.y) ||
			!Equals(z, RHS.z) ||
			!Equals(w, RHS.w);
	}

	vec4& vec4::operator-=(const vec4& RHS)
	{
		this->x -= RHS.x;
		this->y -= RHS.y;
		this->z -= RHS.z;
		this->w -= RHS.w;
		return *this;
	}

	vec4& vec4::operator+=(const vec4& RHS)
	{
		this->x += RHS.x;
		this->y += RHS.y;
		this->z += RHS.z;
		this->w += RHS.w;
		return *this;
	}

	vec4& vec4::operator*=(const vec4& RHS)
	{
		this->x *= RHS.x;
		this->y *= RHS.y;
		this->z *= RHS.z;
		this->w *= RHS.w;
		return *this;
	}

	vec4& vec4::operator/=(const vec4& RHS)
	{
		this->x /= RHS.x;
		this->y /= RHS.y;
		this->z /= RHS.z;
		this->w /= RHS.w;
		return *this;
	}

	vec4& vec4::operator*=(float scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
		this->w *= scalar;
		return *this;
	}

	vec4& vec4::operator/=(float divisor)
	{
		float inverse = 1.0f / divisor;
		this->x *= inverse;
		this->y *= inverse;
		this->z *= inverse;
		this->w *= inverse;
		return *this;
	}

	vec4 vec4::operator-() const
	{
		return { -x, -y, -z, -w };
	}

	vec4 vec4::operator-(const vec4& RHS) const
	{
		return { x - RHS.x, y - RHS.y, z - RHS.z, w - RHS.w };
	}

	vec4 vec4::operator+(const vec4& RHS) const
	{
		return { x + RHS.x, y + RHS.y, z + RHS.z, w + RHS.w };
	}

	vec4 vec4::operator*(const vec4& RHS) const
	{
		return { x * RHS.x, y * RHS.y, z * RHS.z, w * RHS.w };
	}

	vec4 vec4::operator/(const vec4& RHS) const
	{
		return { x / RHS.x, y / RHS.y, z / RHS.z, w / RHS.w };
	}

	vec4 vec4::operator*(float scalar) const
	{
		return { x * scalar, y * scalar, z * scalar, w * scalar };
	}

	vec4 vec4::operator/(float divisor) const
	{
		float inverse = 1.0f / divisor;
		return { x * inverse, y * inverse, z * inverse, w * inverse };
	}

	float vec4::operator[](unsigned index) const
	{
		ASSERT(index < 4, "'index' must be in the range of [0, 3].");
		return *(&x + index);
	}

	float& vec4::operator[](unsigned index)
	{
		ASSERT(index < 4, "'index' must be in the range of [0, 3].");
		return *(&x + index);
	}

	vec4::operator vec2() const
	{
		return { x, y };
	}

	vec4::operator vec3() const
	{
		return { x, y, z };
	}

	float Length(const vec2& v)
	{
		return sqrt(v.x * v.x + v.y * v.y);
	}

	float Length(const vec3& v)
	{
		return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	float Length(const vec4& v)
	{
		return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}

	float LengthSquared(const vec2& v)
	{
		return (v.x * v.x) + (v.y * v.y);
	}

	float LengthSquared(const vec3& v)
	{
		return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
	}

	float LengthSquared(const vec4& v)
	{
		return (v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w);
	}

	float Distance(const vec2& v1, const vec2& v2)
	{
		return Length(v1 - v2);
	}

	float Distance(const vec3& v1, const vec3& v2)
	{
		return Length(v1 - v2);
	}

	float Distance(const vec4& v1, const vec4& v2)
	{
		return Length(v1 - v2);
	}

	float DistanceSquared(const vec2& v1, const vec2& v2)
	{
		return LengthSquared(v1 - v2);
	}

	float DistanceSquared(const vec3& v1, const vec3& v2)
	{
		return LengthSquared(v1 - v2);
	}

	float DistanceSquared(const vec4& v1, const vec4& v2)
	{
		return LengthSquared(v1 - v2);
	}

	float Dot(const vec2& v1, const vec2& v2)
	{
		return (v1.x * v2.x) + (v1.y * v2.y);
	}

	float Dot(const vec3& v1, const vec3& v2)
	{
		return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	}

	float Dot(const vec4& v1, const vec4& v2)
	{
		return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
	}

	bool Parallel(const vec2& v1, const vec2& v2)
	{
		ASSERT(Equals(Length(v1), 1.0f), "'v1' vector must be normalized.");
		ASSERT(Equals(Length(v2), 1.0f), "'v2' vector must be normalized.");

		return Equals(Abs(Dot(v1, v2)), 1.0f, 1.e-6f);
	}

	bool Parallel(const vec3& v1, const vec3& v2)
	{
		ASSERT(Equals(Length(v1), 1.0f), "'v1' vector must be normalized.");
		ASSERT(Equals(Length(v2), 1.0f), "'v2' vector must be normalized.");

		return Equals(Abs(Dot(v1, v2)), 1.0f, 1.e-6f);
	}

	bool Parallel(const vec4& v1, const vec4& v2)
	{
		ASSERT(Equals(Length(v1), 1.0f), "'v1' vector must be normalized.");
		ASSERT(Equals(Length(v2), 1.0f), "'v2' vector must be normalized.");

		return Equals(Abs(Dot(v1, v2)), 1.0f, 1.e-6f);
	}

	bool Orthogonal(const vec2& v1, const vec2& v2)
	{
		ASSERT(Equals(Length(v1), 1.0f), "'v1' vector must be normalized.");
		ASSERT(Equals(Length(v2), 1.0f), "'v2' vector must be normalized.");

		return Equals(Dot(v1, v2), 0.0f, 1.e-6f);
	}

	bool Orthogonal(const vec3& v1, const vec3& v2)
	{
		ASSERT(Equals(Length(v1), 1.0f), "'v1' vector must be normalized.");
		ASSERT(Equals(Length(v2), 1.0f), "'v2' vector must be normalized.");

		return Equals(Dot(v1, v2), 0.0f, 1.e-6f);
	}

	bool Orthogonal(const vec4& v1, const vec4& v2)
	{
		ASSERT(Equals(Length(v1), 1.0f), "'v1' vector must be normalized.");
		ASSERT(Equals(Length(v2), 1.0f), "'v2' vector must be normalized.");

		return Equals(Dot(v1, v2), 0.0f, 1.e-6f);
	}

	vec2 Normalize(const vec2& v)
	{
		float length = Length(v);
		ASSERT(!Equals(length, 0.0f), "Zero length vector cannot be normalized.");

		float invLength = 1.0f / length;
		return v * invLength;
	}

	vec3 Normalize(const vec3& v)
	{
		float length = Length(v);
		ASSERT(!Equals(length, 0.0f), "Zero length vector cannot be normalized.");

		float invLength = 1.0f / length;
		return v * invLength;
	}

	vec4 Normalize(const vec4& v)
	{
		float length = Length(v);
		ASSERT(!Equals(length, 0.0f), "Zero length vector cannot be normalized.");

		float invLength = 1.0f / length;
		return v * invLength;
	}

	vec2 NormalizeSafe(const vec2& v)
	{
		float length = Length(v);
		if (Equals(length, 0.0f))
		{
			return vec2::Zero;
		}

		float invLength = 1.0f / length;
		return v * invLength;
	}

	vec3 NormalizeSafe(const vec3& v)
	{
		float length = Length(v);
		if (Equals(length, 0.0f))
		{
			return vec3::Zero;
		}

		float invLength = 1.0f / length;
		return v * invLength;
	}

	vec4 NormalizeSafe(const vec4& v)
	{
		float length = Length(v);
		if (Equals(length, 0.0f))
		{
			return vec4::Zero;
		}

		float invLength = 1.0f / length;
		return v * invLength;
	}

	vec2 SnapLength(const vec2& v, float step)
	{
		float length = Length(v);
		float target = SnapToGrid(length, step);
		ASSERT(!Equals(length, 0.0f), "Zero length vector cannot be snapped to length.");

		return v * (target / length);
	}

	vec3 SnapLength(const vec3& v, float step)
	{
		float length = Length(v);
		float target = SnapToGrid(length, step);
		ASSERT(!Equals(length, 0.0f), "Zero length vector cannot be snapped to length.");

		return v * (target / length);
	}

	vec4 SnapLength(const vec4& v, float step)
	{
		float length = Length(v);
		float target = SnapToGrid(length, step);
		ASSERT(!Equals(length, 0.0f), "Zero length vector cannot be snapped to length.");

		return v * (target / length);
	}

	vec2 SnapLengthSafe(const vec2& v, float step)
	{
		float length = Length(v);
		if (Equals(length, 0.0f))
		{
			return vec2::Zero;
		}

		float target = SnapToGrid(length, step);
		return v * (target / length);
	}

	vec3 SnapLengthSafe(const vec3& v, float step)
	{
		float length = Length(v);
		if (Equals(length, 0.0f))
		{
			return vec3::Zero;
		}

		float target = SnapToGrid(length, step);
		return v * (target / length);
	}

	vec4 SnapLengthSafe(const vec4& v, float step)
	{
		float length = Length(v);
		if (Equals(length, 0.0f))
		{
			return vec4::Zero;
		}

		float target = SnapToGrid(length, step);
		return v * (target / length);
	}

	vec3 Cross(const vec3& v1, const vec3& v2)
	{
		return vec3(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x);
	}

	vec2 Reflect(const vec2& incident, const vec2& normal)
	{
		ASSERT(Equals(Length(normal), 1.0f), "'normal' vector must be normalized.");

		return incident - 2.0f * Dot(normal, incident) * normal;
	}

	vec3 Reflect(const vec3& incident, const vec3& normal)
	{
		ASSERT(Equals(Length(normal), 1.0f), "'normal' vector must be normalized.");

		return incident - 2.0f * Dot(normal, incident) * normal;
	}

	vec4 Reflect(const vec4& incident, const vec4& normal)
	{
		ASSERT(Equals(Length(normal), 1.0f), "'normal' vector must be normalized.");

		return incident - 2.0f * Dot(normal, incident) * normal;
	}

	vec2 Refract(const vec2& incident, const vec2& normal, float index)
	{
		ASSERT(Equals(Length(normal), 1.0f), "'normal' vector must be normalized.");

		vec2 result;

		float dot = Dot(normal, incident);
		float k = 1.0f - index * index * (1.0f - dot * dot);

		if (k >= 0.0f)
		{
			result = index * incident - (index * dot + sqrt(k)) * normal;
		}

		return result;
	}

	vec3 Refract(const vec3& incident, const vec3& normal, float index)
	{
		ASSERT(Equals(Length(normal), 1.0f), "'normal' vector must be normalized.");

		vec3 result;

		float dot = Dot(normal, incident);
		float k = 1.0f - index * index * (1.0f - dot * dot);

		if (k >= 0.0f)
		{
			result = index * incident - (index * dot + sqrt(k)) * normal;
		}

		return result;
	}

	vec4 Refract(const vec4& incident, const vec4& normal, float index)
	{
		ASSERT(Equals(Length(normal), 1.0f), "'normal' vector must be normalized.");

		vec4 result;

		float dot = Dot(normal, incident);
		float k = 1.0f - index * index * (1.0f - dot * dot);

		if (k >= 0.0f)
		{
			result = index * incident - (index * dot + sqrt(k)) * normal;
		}

		return result;
	}

	vec2 Abs(const vec2& v)
	{
		return { Abs(v.x), Abs(v.y) };
	}

	vec3 Abs(const vec3& v)
	{
		return { Abs(v.x), Abs(v.y), Abs(v.z) };
	}

	vec4 Abs(const vec4& v)
	{
		return { Abs(v.x), Abs(v.y), Abs(v.z), Abs(v.w) };
	}

	vec2 Min(const vec2& v1, const vec2& v2)
	{
		return vec2(
			Min(v1.x, v2.x),
			Min(v1.y, v2.y));
	}

	vec3 Min(const vec3& v1, const vec3& v2)
	{
		return vec3(
			Min(v1.x, v2.x),
			Min(v1.y, v2.y),
			Min(v1.z, v2.z));
	}

	vec4 Min(const vec4& v1, const vec4& v2)
	{
		return vec4(
			Min(v1.x, v2.x),
			Min(v1.y, v2.y),
			Min(v1.z, v2.z),
			Min(v1.w, v2.w));
	}

	vec2 Max(const vec2& v1, const vec2& v2)
	{
		return vec2(
			Max(v1.x, v2.x),
			Max(v1.y, v2.y));
	}

	vec3 Max(const vec3& v1, const vec3& v2)
	{
		return vec3(
			Max(v1.x, v2.x),
			Max(v1.y, v2.y),
			Max(v1.z, v2.z));
	}

	vec4 Max(const vec4& v1, const vec4& v2)
	{
		return vec4(
			Max(v1.x, v2.x),
			Max(v1.y, v2.y),
			Max(v1.z, v2.z),
			Max(v1.w, v2.w));
	}

	vec2 Clamp(const vec2& vec, const vec2& min, const vec2& max)
	{
		return vec2(
			Clamp(vec.x, min.x, max.x),
			Clamp(vec.y, min.y, max.y));
	}

	vec3 Clamp(const vec3& vec, const vec3& min, const vec3& max)
	{
		return vec3(
			Clamp(vec.x, min.x, max.x),
			Clamp(vec.y, min.y, max.y),
			Clamp(vec.z, min.z, max.z));
	}

	vec4 Clamp(const vec4& vec, const vec4& min, const vec4& max)
	{
		return vec4(
			Clamp(vec.x, min.x, max.x),
			Clamp(vec.y, min.y, max.y),
			Clamp(vec.z, min.z, max.z),
			Clamp(vec.w, min.w, max.w));
	}

	vec2 operator*(float scalar, const vec2& vec)
	{
		return { vec.x * scalar, vec.y * scalar };
	}

	vec2 operator/(float divisor, const vec2& vec)
	{
		float inverse = 1.0f / divisor;
		return { vec.x * inverse, vec.y * inverse };
	}

	vec3 operator*(float scalar, const vec3& vec)
	{
		return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
	}

	vec3 operator/(float divisor, const vec3& vec)
	{
		float inverse = 1.0f / divisor;
		return { vec.x * inverse, vec.y * inverse, vec.z * inverse };
	}

	vec4 operator*(float scalar, const vec4& vec)
	{
		return { vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar };
	}

	vec4 operator/(float divisor, const vec4& vec)
	{
		float inverse = 1.0f / divisor;
		return { vec.x * inverse, vec.y * inverse, vec.z * inverse, vec.w * inverse };
	}
}

REFLECT(gem::vec2)
	MEMBERS {
		REF_MEMBER(x)
		REF_MEMBER(y)
	}
REF_END;

REFLECT(gem::vec3)
	MEMBERS {
		REF_MEMBER(x)
		REF_MEMBER(y)
		REF_MEMBER(z)
	}
REF_END;

REFLECT(gem::vec4)
	MEMBERS {
		REF_MEMBER(x)
		REF_MEMBER(y)
		REF_MEMBER(z)
		REF_MEMBER(w)
	}
REF_END;
