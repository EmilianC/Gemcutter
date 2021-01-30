// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace Jwl
{
	struct vec2
	{
		vec2() = default;
		vec2(float x, float y);
		explicit vec2(float val);

		bool operator==(const vec2&) const;
		bool operator!=(const vec2&) const;

		vec2& operator-=(const vec2&);
		vec2& operator+=(const vec2&);
		vec2& operator*=(const vec2&);
		vec2& operator/=(const vec2&);
		vec2& operator*=(float scalar);
		vec2& operator/=(float divisor);

		vec2 operator-() const;
		vec2 operator-(const vec2&) const;
		vec2 operator+(const vec2&) const;
		vec2 operator*(const vec2&) const;
		vec2 operator/(const vec2&) const;
		vec2 operator*(float scalar) const;
		vec2 operator/(float divisor) const;

		float operator[](unsigned index) const;
		float& operator[](unsigned index);

		static const vec2 Zero;
		static const vec2 One;
		static const vec2 Right;
		static const vec2 Up;

		float x = 0.0f;
		float y = 0.0f;
	};

	struct vec3
	{
		vec3() = default;
		vec3(const vec2& xy, float z);
		vec3(float x, float y, float z);
		explicit vec3(float val);

		bool operator==(const vec3&) const;
		bool operator!=(const vec3&) const;

		vec3& operator-=(const vec3&);
		vec3& operator+=(const vec3&);
		vec3& operator*=(const vec3&);
		vec3& operator/=(const vec3&);
		vec3& operator*=(float scalar);
		vec3& operator/=(float divisor);

		vec3 operator-() const;
		vec3 operator-(const vec3&) const;
		vec3 operator+(const vec3&) const;
		vec3 operator*(const vec3&) const;
		vec3 operator/(const vec3&) const;
		vec3 operator*(float scalar) const;
		vec3 operator/(float divisor) const;

		float operator[](unsigned index) const;
		float& operator[](unsigned index);

		explicit operator vec2() const;

		static const vec3 Zero;
		static const vec3 One;
		static const vec3 Right;
		static const vec3 Up;
		static const vec3 Forward;

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	struct vec4
	{
		vec4() = default;
		vec4(const vec3& xyz, float w);
		vec4(const vec2& xy, float y, float w);
		vec4(const vec2& xy, const vec2& zw);
		vec4(float x, float y, float z, float w);
		explicit vec4(float val);

		bool operator==(const vec4&) const;
		bool operator!=(const vec4&) const;

		vec4& operator-=(const vec4&);
		vec4& operator+=(const vec4&);
		vec4& operator*=(const vec4&);
		vec4& operator/=(const vec4&);
		vec4& operator*=(float scalar);
		vec4& operator/=(float divisor);

		vec4 operator-() const;
		vec4 operator-(const vec4&) const;
		vec4 operator+(const vec4&) const;
		vec4 operator*(const vec4&) const;
		vec4 operator/(const vec4&) const;
		vec4 operator*(float scalar) const;
		vec4 operator/(float divisor) const;

		float operator[](unsigned index) const;
		float& operator[](unsigned index);

		explicit operator vec2() const;
		explicit operator vec3() const;

		static const vec4 Zero;
		static const vec4 One;
		static const vec4 Right;
		static const vec4 Up;
		static const vec4 Forward;

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;
	};

	float Length(const vec2&);
	float Length(const vec3&);
	float Length(const vec4&);
	float LengthSquared(const vec2&);
	float LengthSquared(const vec3&);
	float LengthSquared(const vec4&);
	float Distance(const vec2&, const vec2&);
	float Distance(const vec3&, const vec3&);
	float Distance(const vec4&, const vec4&);
	float Dot(const vec2&, const vec2&);
	float Dot(const vec3&, const vec3&);
	float Dot(const vec4&, const vec4&);
	[[nodiscard]] vec2 Normalize(const vec2&);
	[[nodiscard]] vec3 Normalize(const vec3&);
	[[nodiscard]] vec4 Normalize(const vec4&);
	vec3 Cross(const vec3&, const vec3&);
	vec2 Reflect(const vec2& incident, const vec2& normal);
	vec3 Reflect(const vec3& incident, const vec3& normal);
	vec4 Reflect(const vec4& incident, const vec4& normal);
	vec2 Refract(const vec2& incident, const vec2& normal, float index);
	vec3 Refract(const vec3& incident, const vec3& normal, float index);
	vec4 Refract(const vec4& incident, const vec4& normal, float index);
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

	vec2 operator*(float scalar, const vec2&);
	vec2 operator/(float divisor, const vec2&);
	vec3 operator*(float scalar, const vec3&);
	vec3 operator/(float divisor, const vec3&);
	vec4 operator*(float scalar, const vec4&);
	vec4 operator/(float divisor, const vec4&);
}
