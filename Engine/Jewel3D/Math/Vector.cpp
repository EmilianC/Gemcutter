// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Vector.h"
#include "Math.h"
#include "Jewel3D/Application/Logging.h"

namespace Jwl
{

#pragma region Vector2

	const vec2 vec2::Zero = vec2(0.0f, 0.0f);
	const vec2 vec2::One = vec2(1.0f, 1.0f);
	const vec2 vec2::Right = vec2(1.0f, 0.0f);
	const vec2 vec2::Up = vec2(0.0f, 1.0f);

	vec2::vec2(f32 x, f32 y)
		: x(x), y(y)
	{
	}

	vec2::vec2(f32 val)
		: x(val), y(val)
	{
	}

	bool vec2::operator==(const vec2 &RHS) const
	{
		return (this->x == RHS.x) && (this->y == RHS.y);
	}

	bool vec2::operator!=(const vec2 &RHS) const
	{
		return (this->x != RHS.x) || (this->y != RHS.y);
	}

	vec2& vec2::operator=(const vec2 &RHS)
	{
		this->x = RHS.x;
		this->y = RHS.y;
		return *this;
	}

	vec2& vec2::operator-=(const vec2 &RHS)
	{
		this->x -= RHS.x;
		this->y -= RHS.y;
		return *this;
	}

	vec2& vec2::operator+=(const vec2 &RHS)
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

	vec2& vec2::operator*=(f32 scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		return *this;
	}

	vec2& vec2::operator/=(f32 divisor)
	{
		f32 inverse = 1.0f / divisor;
		this->x *= inverse;
		this->y *= inverse;
		return *this;
	}

	vec2 vec2::operator-() const
	{
		return vec2(-x, -y);
	}

	vec2 vec2::operator-(const vec2 &RHS) const
	{
		return vec2(x - RHS.x, y - RHS.y);
	}

	vec2 vec2::operator+(const vec2 &RHS) const
	{
		return vec2(x + RHS.x, y + RHS.y);
	}

	vec2 vec2::operator*(const vec2 &RHS) const
	{
		return vec2(x * RHS.x, y * RHS.y);
	}

	vec2 vec2::operator/(const vec2 &RHS) const
	{
		return vec2(x / RHS.x, y / RHS.y);
	}

	vec2 vec2::operator*(f32 scalar) const
	{
		return vec2(x * scalar, y * scalar);
	}

	vec2 vec2::operator/(f32 divisor) const
	{
		f32 inverse = 1.0f / divisor;
		return vec2(x * inverse, y * inverse);
	}

	f32 vec2::operator[](u32 index) const
	{
		ASSERT(index >= 0 && index < 2, "'index' must be in the range of [0, 1].");
		return *(&x + index);
	}

	f32 &vec2::operator[](u32 index)
	{
		ASSERT(index >= 0 && index < 2, "'index' must be in the range of [0, 1].");
		return *(&x + index);
	}

	f32 vec2::Length() const
	{
		return sqrt(x * x + y * y);
	}

	f32 vec2::LengthSquared() const
	{
		return x * x + y * y;
	}

	void vec2::ClampLength(f32 length)
	{
		ASSERT(length >= 0.0f, "'length' must be positive.");

		f32 magnitude = Length();
		if (magnitude > length)
		{
			*this *= (length / magnitude);
		}
	}

	void vec2::Normalize()
	{
		f32 invLength = 1.0f / Length();

		ASSERT(!std::isinf(invLength), "Zero length vector cannot be normalized.");

		x *= invLength;
		y *= invLength;
	}

	vec2 vec2::GetNormalized() const
	{
		f32 invLength = 1.0f / Length();

		ASSERT(!std::isinf(invLength), "Zero length vector cannot be normalized.");

		return vec2(x * invLength, y * invLength);
	}

#pragma endregion

#pragma region Vector3

	const vec3 vec3::Zero = vec3(0.0f, 0.0f, 0.0f);
	const vec3 vec3::One = vec3(1.0f, 1.0f, 1.0f);
	const vec3 vec3::Right = vec3(1.0f, 0.0f, 0.0f);
	const vec3 vec3::Up = vec3(0.0f, 1.0f, 0.0f);
	const vec3 vec3::Forward = vec3(0.0f, 0.0f, 1.0f);

	vec3::vec3(const vec2 &xy, f32 z)
		: x(xy[0]), y(xy[1]), z(z)
	{
	}

	vec3::vec3(f32 x, f32 y, f32 z)
		: x(x), y(y), z(z)
	{
	}

	vec3::vec3(f32 val)
		: x(val), y(val), z(val)
	{
	}

	bool vec3::operator==(const vec3 &RHS) const
	{
		return (this->x == RHS.x) && (this->y == RHS.y) && (this->z == RHS.z);
	}

	bool vec3::operator!=(const vec3 &RHS) const
	{
		return (this->x != RHS.x) || (this->y != RHS.y) || (this->z != RHS.z);
	}

	vec3& vec3::operator=(const vec3 &RHS)
	{
		this->x = RHS.x;
		this->y = RHS.y;
		this->z = RHS.z;
		return *this;
	}

	vec3& vec3::operator-=(const vec3 &RHS)
	{
		this->x -= RHS.x;
		this->y -= RHS.y;
		this->z -= RHS.z;
		return *this;
	}

	vec3& vec3::operator+=(const vec3 &RHS)
	{
		this->x += RHS.x;
		this->y += RHS.y;
		this->z += RHS.z;
		return *this;
	}

	vec3& vec3::operator*=(const vec3 &RHS)
	{
		this->x *= RHS.x;
		this->y *= RHS.y;
		this->z *= RHS.z;
		return *this;
	}

	vec3& vec3::operator/=(const vec3 &RHS)
	{
		this->x /= RHS.x;
		this->y /= RHS.y;
		this->z /= RHS.z;
		return *this;
	}

	vec3& vec3::operator*=(f32 scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
		return *this;
	}

	vec3& vec3::operator/=(f32 divisor)
	{
		f32 inverse = 1.0f / divisor;
		this->x *= inverse;
		this->y *= inverse;
		this->z *= inverse;
		return *this;
	}

	vec3 vec3::operator-() const
	{
		return vec3(-x, -y, -z);
	}

	vec3 vec3::operator-(const vec3 &RHS) const
	{
		return vec3(x - RHS.x, y - RHS.y, z - RHS.z);
	}

	vec3 vec3::operator+(const vec3 &RHS) const
	{
		return vec3(x + RHS.x, y + RHS.y, z + RHS.z);
	}

	vec3 vec3::operator*(const vec3 &RHS) const
	{
		return vec3(x * RHS.x, y * RHS.y, z * RHS.z);
	}

	vec3 vec3::operator/(const vec3 &RHS) const
	{
		return vec3(x / RHS.x, y / RHS.y, z / RHS.z);
	}

	vec3 vec3::operator*(f32 scalar) const
	{
		return vec3(x * scalar, y * scalar, z * scalar);
	}

	vec3 vec3::operator/(f32 divisor) const
	{
		f32 inverse = 1.0f / divisor;
		return vec3(x * inverse, y * inverse, z * inverse);
	}

	f32 vec3::operator[](u32 index) const
	{
		ASSERT(index >= 0 && index < 3, "'index' must be in the range of [0, 2].");
		return *(&x + index);
	}

	f32 &vec3::operator[](u32 index)
	{
		ASSERT(index >= 0 && index < 3, "'index' must be in the range of [0, 2].");
		return *(&x + index);
	}

	f32 vec3::Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	f32 vec3::LengthSquared() const
	{
		return x * x + y * y + z * z;
	}

	void vec3::ClampLength(f32 length)
	{
		ASSERT(length >= 0.0f, "'length' must be positive.");

		f32 magnitude = Length();
		if (magnitude > length)
		{
			*this *= (length / magnitude);
		}
	}

	void vec3::Normalize()
	{
		f32 invLength = 1.0f / Length();

		ASSERT(!std::isinf(invLength), "Zero length vector cannot be normalized.");

		x *= invLength;
		y *= invLength;
		z *= invLength;
	}

	vec3 vec3::GetNormalized() const
	{
		f32 invLength = 1.0f / Length();

		ASSERT(!std::isinf(invLength), "Zero length vector cannot be normalized.");

		return vec3(x * invLength, y * invLength, z * invLength);
	}

	vec2 vec3::ToVec2() const
	{
		return vec2(x, y);
	}

#pragma endregion

#pragma region vector4

	const vec4 vec4::Zero = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	const vec4 vec4::One = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	const vec4 vec4::Right = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	const vec4 vec4::Up = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	const vec4 vec4::Forward = vec4(0.0f, 0.0f, 1.0f, 0.0f);

	vec4::vec4(const vec2 &xy, f32 z, f32 w)
		: x(xy[0]), y(xy[1]), z(z), w(w)
	{
	}

	vec4::vec4(const vec2& xy, const vec2& zw)
		: x(xy[0]), y(xy[1]), z(zw[0]), w(zw[1])
	{
	}

	vec4::vec4(const vec3 &xyz, f32 w)
		: x(xyz[0]), y(xyz[1]), z(xyz[2]), w(w)
	{
	}

	vec4::vec4(f32 x, f32 y, f32 z, f32 w)
		: x(x), y(y), z(z), w(w)
	{
	}

	vec4::vec4(f32 val)
		: x(val), y(val), z(val), w(val)
	{
	}

	bool vec4::operator==(const vec4 &RHS) const
	{
		return (this->x == RHS.x) && (this->y == RHS.y) && (this->z == RHS.z) && (this->w == RHS.w);
	}

	bool vec4::operator!=(const vec4 &RHS) const
	{
		return (this->x != RHS.x) || (this->y != RHS.y) || (this->z != RHS.z) || (this->w != RHS.w);
	}

	vec4& vec4::operator=(const vec4 &RHS)
	{
		this->x = RHS.x;
		this->y = RHS.y;
		this->z = RHS.z;
		this->w = RHS.w;
		return *this;
	}

	vec4& vec4::operator-=(const vec4 &RHS)
	{
		this->x -= RHS.x;
		this->y -= RHS.y;
		this->z -= RHS.z;
		this->w -= RHS.w;
		return *this;
	}

	vec4& vec4::operator+=(const vec4 &RHS)
	{
		this->x += RHS.x;
		this->y += RHS.y;
		this->z += RHS.z;
		this->w += RHS.w;
		return *this;
	}

	vec4& vec4::operator*=(const vec4 &RHS)
	{
		this->x *= RHS.x;
		this->y *= RHS.y;
		this->z *= RHS.z;
		this->w *= RHS.w;
		return *this;
	}

	vec4& vec4::operator/=(const vec4 &RHS)
	{
		this->x /= RHS.x;
		this->y /= RHS.y;
		this->z /= RHS.z;
		this->w /= RHS.w;
		return *this;
	}

	vec4& vec4::operator*=(f32 scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
		this->w *= scalar;
		return *this;
	}

	vec4& vec4::operator/=(f32 divisor)
	{
		f32 inverse = 1.0f / divisor;
		this->x *= inverse;
		this->y *= inverse;
		this->z *= inverse;
		this->w *= inverse;
		return *this;
	}

	vec4 vec4::operator-() const
	{
		return vec4(-x, -y, -z, -w);
	}

	vec4 vec4::operator-(const vec4 &RHS) const
	{
		return vec4(x - RHS.x, y - RHS.y, z - RHS.z, w - RHS.w);
	}

	vec4 vec4::operator+(const vec4 &RHS) const
	{
		return vec4(x + RHS.x, y + RHS.y, z + RHS.z, w + RHS.w);
	}

	vec4 vec4::operator*(const vec4 &RHS) const
	{
		return vec4(x * RHS.x, y * RHS.y, z * RHS.z, w * RHS.w);
	}

	vec4 vec4::operator/(const vec4 &RHS) const
	{
		return vec4(x / RHS.x, y / RHS.y, z / RHS.z, w / RHS.w);
	}

	vec4 vec4::operator*(f32 scalar) const
	{
		return vec4(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	vec4 vec4::operator/(f32 divisor) const
	{
		f32 inverse = 1.0f / divisor;
		return vec4(x * inverse, y * inverse, z * inverse, w * inverse);
	}

	f32 vec4::operator[](u32 index) const
	{
		ASSERT(index >= 0 && index < 4, "'index' must be in the range of [0, 3].");
		return *(&x + index);
	}

	f32 &vec4::operator[](u32 index)
	{
		ASSERT(index >= 0 && index < 4, "'index' must be in the range of [0, 3].");
		return *(&x + index);
	}

	f32 vec4::Length() const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	f32 vec4::LengthSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

	void vec4::ClampLength(f32 length)
	{
		ASSERT(length >= 0.0f, "'length' must be positive.");

		f32 magnitude = Length();
		if (magnitude > length)
		{
			*this *= (length / magnitude);
		}
	}

	void vec4::Normalize()
	{
		f32 invLength = 1.0f / Length();

		ASSERT(!std::isinf(invLength), "Zero length vector cannot be normalized.");

		x *= invLength;
		y *= invLength;
		z *= invLength;
		w *= invLength;
	}

	vec4 vec4::GetNormalized() const
	{
		f32 invLength = 1.0f / Length();

		ASSERT(!std::isinf(invLength), "Zero length vector cannot be normalized.");

		return vec4(x * invLength, y * invLength, z * invLength, w * invLength);
	}

	vec3 vec4::ToVec3() const
	{
		return vec3(x, y, z);
	}

#pragma endregion

	f32 Distance(const vec2 &v1, const vec2 &v2)
	{
		return (v1 - v2).Length();
	}

	f32 Distance(const vec3 &v1, const vec3 &v2)
	{
		return (v1 - v2).Length();
	}

	f32 Distance(const vec4 &v1, const vec4 &v2)
	{
		return (v1 - v2).Length();
	}

	f32 Dot(const vec2 &v1, const vec2 &v2)
	{
		return (v1.x * v2.x) + (v1.y * v2.y);
	}

	f32 Dot(const vec3 &v1, const vec3 &v2)
	{
		return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	}

	f32 Dot(const vec4 &v1, const vec4 &v2)
	{
		return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
	}

	vec3 Cross(const vec3 &v1, const vec3 &v2)
	{
		return vec3(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x);
	}
	
	vec2 Reflect(const vec2& incident, const vec2& normal)
	{
		return incident - 2.0f * Dot(normal, incident) * normal;
	}

	vec3 Reflect(const vec3& incident, const vec3& normal)
	{
		return incident - 2.0f * Dot(normal, incident) * normal;
	}

	vec4 Reflect(const vec4& incident, const vec4& normal)
	{
		return incident - 2.0f * Dot(normal, incident) * normal;
	}

	vec2 Refract(const vec2& incident, const vec2& normal, f32 index)
	{
		vec2 result;

		f32 dot = Dot(normal, incident);
		f32 k = 1.0f - index * index * (1.0f - dot * dot);

		if (k >= 0.0f)
		{
			result = index * incident - (index * dot + sqrt(k)) * normal;
		}

		return result;
	}

	vec3 Refract(const vec3& incident, const vec3& normal, f32 index)
	{
		vec3 result;

		f32 dot = Dot(normal, incident);
		f32 k = 1.0f - index * index * (1.0f - dot * dot);

		if (k >= 0.0f)
		{
			result = index * incident - (index * dot + sqrt(k)) * normal;
		}

		return result;
	}

	vec4 Refract(const vec4& incident, const vec4& normal, f32 index)
	{
		vec4 result;

		f32 dot = Dot(normal, incident);
		f32 k = 1.0f - index * index * (1.0f - dot * dot);

		if (k >= 0.0f)
		{
			result = index * incident - (index * dot + sqrt(k)) * normal;
		}

		return result;
	}

	vec2 Abs(const vec2& v)
	{
		return vec2(Abs(v.x), Abs(v.y));
	}

	vec3 Abs(const vec3& v)
	{
		return vec3(Abs(v.x), Abs(v.y), Abs(v.z));
	}

	vec4 Abs(const vec4& v)
	{
		return vec4(Abs(v.x), Abs(v.y), Abs(v.z), Abs(v.w));
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

	vec2 operator*(f32 scalar, const vec2& vec)
	{
		return vec2(vec.x * scalar, vec.y * scalar);
	}

	vec2 operator/(f32 divisor, const vec2& vec)
	{
		f32 inverse = 1.0f / divisor;
		return vec2(vec.x * inverse, vec.y * inverse);
	}
	
	vec3 operator*(f32 scalar, const vec3& vec)
	{
		return vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
	}

	vec3 operator/(f32 divisor, const vec3& vec)
	{
		f32 inverse = 1.0f / divisor;
		return vec3(vec.x * inverse, vec.y * inverse, vec.z * inverse);
	}

	vec4 operator*(f32 scalar, const vec4& vec)
	{
		return vec4(vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar);
	}

	vec4 operator/(f32 divisor, const vec4& vec)
	{
		f32 inverse = 1.0f / divisor;
		return vec4(vec.x * inverse, vec.y * inverse, vec.z * inverse, vec.w * inverse);
	}
}
