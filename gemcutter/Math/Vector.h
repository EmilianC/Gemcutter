// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace gem
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
		static const vec2 Left;
		static const vec2 Right;
		static const vec2 Up;
		static const vec2 Down;

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
		static const vec3 Left;
		static const vec3 Right;
		static const vec3 Up;
		static const vec3 Down;
		static const vec3 Forward;
		static const vec3 Backward;

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
		static const vec4 Left;
		static const vec4 Right;
		static const vec4 Up;
		static const vec4 Down;
		static const vec4 Forward;
		static const vec4 Backward;

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;
	};

	[[nodiscard]] float Length(const vec2&);
	[[nodiscard]] float Length(const vec3&);
	[[nodiscard]] float Length(const vec4&);
	[[nodiscard]] float LengthSquared(const vec2&);
	[[nodiscard]] float LengthSquared(const vec3&);
	[[nodiscard]] float LengthSquared(const vec4&);
	[[nodiscard]] float Distance(const vec2&, const vec2&);
	[[nodiscard]] float Distance(const vec3&, const vec3&);
	[[nodiscard]] float Distance(const vec4&, const vec4&);
	[[nodiscard]] float DistanceSquared(const vec2&, const vec2&);
	[[nodiscard]] float DistanceSquared(const vec3&, const vec3&);
	[[nodiscard]] float DistanceSquared(const vec4&, const vec4&);
	[[nodiscard]] float Dot(const vec2&, const vec2&);
	[[nodiscard]] float Dot(const vec3&, const vec3&);
	[[nodiscard]] float Dot(const vec4&, const vec4&);
	[[nodiscard]] vec2 Normalize(const vec2&);
	[[nodiscard]] vec3 Normalize(const vec3&);
	[[nodiscard]] vec4 Normalize(const vec4&);
	[[nodiscard]] vec2 NormalizeSafe(const vec2&);
	[[nodiscard]] vec3 NormalizeSafe(const vec3&);
	[[nodiscard]] vec4 NormalizeSafe(const vec4&);
	[[nodiscard]] vec2 SnapLength(const vec2&, float step);
	[[nodiscard]] vec3 SnapLength(const vec3&, float step);
	[[nodiscard]] vec4 SnapLength(const vec4&, float step);
	[[nodiscard]] vec2 SnapLengthSafe(const vec2&, float step);
	[[nodiscard]] vec3 SnapLengthSafe(const vec3&, float step);
	[[nodiscard]] vec4 SnapLengthSafe(const vec4&, float step);
	[[nodiscard]] vec3 Cross(const vec3&, const vec3&);
	[[nodiscard]] vec2 Reflect(const vec2& incident, const vec2& normal);
	[[nodiscard]] vec3 Reflect(const vec3& incident, const vec3& normal);
	[[nodiscard]] vec4 Reflect(const vec4& incident, const vec4& normal);
	[[nodiscard]] vec2 Refract(const vec2& incident, const vec2& normal, float index);
	[[nodiscard]] vec3 Refract(const vec3& incident, const vec3& normal, float index);
	[[nodiscard]] vec4 Refract(const vec4& incident, const vec4& normal, float index);
	[[nodiscard]] vec2 Abs(const vec2&);
	[[nodiscard]] vec3 Abs(const vec3&);
	[[nodiscard]] vec4 Abs(const vec4&);
	[[nodiscard]] vec2 Min(const vec2&, const vec2&);
	[[nodiscard]] vec3 Min(const vec3&, const vec3&);
	[[nodiscard]] vec4 Min(const vec4&, const vec4&);
	[[nodiscard]] vec2 Max(const vec2&, const vec2&);
	[[nodiscard]] vec3 Max(const vec3&, const vec3&);
	[[nodiscard]] vec4 Max(const vec4&, const vec4&);
	[[nodiscard]] vec2 Clamp(const vec2& vec, const vec2& min, const vec2& max);
	[[nodiscard]] vec3 Clamp(const vec3& vec, const vec3& min, const vec3& max);
	[[nodiscard]] vec4 Clamp(const vec4& vec, const vec4& min, const vec4& max);

	[[nodiscard]] vec2 operator*(float, const vec2&);
	[[nodiscard]] vec2 operator/(float, const vec2&);
	[[nodiscard]] vec3 operator*(float, const vec3&);
	[[nodiscard]] vec3 operator/(float, const vec3&);
	[[nodiscard]] vec4 operator*(float, const vec4&);
	[[nodiscard]] vec4 operator/(float, const vec4&);
}
