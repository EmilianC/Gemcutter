// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace Jwl
{
	class vec2
	{
	public:
		vec2() = default;
		vec2(f32 x, f32 y);
		explicit vec2(f32 val);

		bool operator==(const vec2&) const;
		bool operator!=(const vec2&) const;

		vec2& operator=(const vec2&);
		vec2& operator-=(const vec2&);
		vec2& operator+=(const vec2&);
		vec2& operator*=(const vec2&);
		vec2& operator/=(const vec2&);
		vec2& operator*=(f32 scalar);
		vec2& operator/=(f32 divisor);

		vec2 operator-() const;
		vec2 operator-(const vec2&) const;
		vec2 operator+(const vec2&) const;
		vec2 operator*(const vec2&) const;
		vec2 operator/(const vec2&) const;
		vec2 operator*(f32 scalar) const;
		vec2 operator/(f32 divisor) const;

		f32 operator[](u32 index) const;
		f32& operator[](u32 index);

		f32 Length() const;
		f32 LengthSquared() const;
		void ClampLength(f32 length);
		void Normalize();
		vec2 GetNormalized() const;

		static const vec2 Zero;
		static const vec2 One;
		static const vec2 Right;
		static const vec2 Up;

		f32 x = 0.0f;
		f32 y = 0.0f;
	};

	class vec3
	{
	public:
		vec3() = default;
		vec3(const vec2& xy, f32 z);
		vec3(f32 x, f32 y, f32 z);
		explicit vec3(f32 val);

		bool operator==(const vec3&) const;
		bool operator!=(const vec3&) const;

		vec3& operator=(const vec3&);
		vec3& operator-=(const vec3&);
		vec3& operator+=(const vec3&);
		vec3& operator*=(const vec3&);
		vec3& operator/=(const vec3&);
		vec3& operator*=(f32 scalar);
		vec3& operator/=(f32 divisor);

		vec3 operator-() const;
		vec3 operator-(const vec3&) const;
		vec3 operator+(const vec3&) const;
		vec3 operator*(const vec3&) const;
		vec3 operator/(const vec3&) const;
		vec3 operator*(f32 scalar) const;
		vec3 operator/(f32 divisor) const;

		f32 operator[](u32 index) const;
		f32& operator[](u32 index);

		f32 Length() const;
		f32 LengthSquared() const;
		void ClampLength(f32 length);
		void Normalize();
		vec3 GetNormalized() const;
		vec2 ToVec2() const;

		static const vec3 Zero;
		static const vec3 One;
		static const vec3 Right;
		static const vec3 Up;
		static const vec3 Forward;

		f32 x = 0.0f;
		f32 y = 0.0f;
		f32 z = 0.0f;
	};

	class vec4
	{
	public:
		vec4() = default;
		vec4(const vec3& xyz, f32 w);
		vec4(const vec2& xy, f32 y, f32 w);
		vec4(const vec2& xy, const vec2& zw);
		vec4(f32 x, f32 y, f32 z, f32 w);
		explicit vec4(f32 val);

		bool operator==(const vec4&) const;
		bool operator!=(const vec4&) const;

		vec4& operator=(const vec4&);
		vec4& operator-=(const vec4&);
		vec4& operator+=(const vec4&);
		vec4& operator*=(const vec4&);
		vec4& operator/=(const vec4&);
		vec4& operator*=(f32 scalar);
		vec4& operator/=(f32 divisor);

		vec4 operator-() const;
		vec4 operator-(const vec4&) const;
		vec4 operator+(const vec4&) const;
		vec4 operator*(const vec4&) const;
		vec4 operator/(const vec4&) const;
		vec4 operator*(f32 scalar) const;
		vec4 operator/(f32 divisor) const;

		f32 operator[](u32 index) const;
		f32& operator[](u32 index);

		f32 Length() const;
		f32 LengthSquared() const;
		void ClampLength(f32 length);
		void Normalize();
		vec4 GetNormalized() const;
		vec3 ToVec3() const;

		static const vec4 Zero;
		static const vec4 One;
		static const vec4 Right;
		static const vec4 Up;
		static const vec4 Forward;

		f32 x = 0.0f;
		f32 y = 0.0f;
		f32 z = 0.0f;
		f32 w = 0.0f;
	};

	f32 Distance(const vec2&, const vec2&);
	f32 Distance(const vec3&, const vec3&);
	f32 Distance(const vec4&, const vec4&);
	f32 Dot(const vec2&, const vec2&);
	f32 Dot(const vec3&, const vec3&);
	f32 Dot(const vec4&, const vec4&);
	vec3 Cross(const vec3&, const vec3&);
	vec2 Reflect(const vec2& incident, const vec2& normal);
	vec3 Reflect(const vec3& incident, const vec3& normal);
	vec4 Reflect(const vec4& incident, const vec4& normal);
	vec2 Refract(const vec2& incident, const vec2& normal, f32 index);
	vec3 Refract(const vec3& incident, const vec3& normal, f32 index);
	vec4 Refract(const vec4& incident, const vec4& normal, f32 index);
	vec2 Abs(const vec2&);
	vec3 Abs(const vec3&);
	vec4 Abs(const vec4&);
	vec2 Min(const vec2&, const vec2&);
	vec3 Min(const vec3&, const vec3&);
	vec4 Min(const vec4&, const vec4&);
	vec2 Max(const vec2&, const vec2&);
	vec3 Max(const vec3&, const vec3&);
	vec4 Max(const vec4&, const vec4&);
	vec2 Clamp(const vec2& vec, const vec2& min, const vec2& max);
	vec3 Clamp(const vec3& vec, const vec3& min, const vec3& max);
	vec4 Clamp(const vec4& vec, const vec4& min, const vec4& max);

	vec2 operator*(f32 scalar, const vec2&);
	vec2 operator/(f32 divisor, const vec2&);
	vec3 operator*(f32 scalar, const vec3&);
	vec3 operator/(f32 divisor, const vec3&);
	vec4 operator*(f32 scalar, const vec4&);
	vec4 operator/(f32 divisor, const vec4&);
}
