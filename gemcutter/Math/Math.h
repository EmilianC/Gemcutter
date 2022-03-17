// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <cmath>

namespace gem
{
	constexpr float M_PI     = 3.14159265358979323846f;
	constexpr float M_E      = 2.71828182845904523536f;
	constexpr float M_LOG2E  = 1.44269504088896340736f;
	constexpr float M_LOG10E = 0.434294481903251827651f;
	constexpr float M_LN2    = 0.693147180559945309417f;
	constexpr float M_LN10   = 2.30258509299404568402f;

	[[nodiscard]]
	constexpr float ToRadian(float degrees)
	{
		return degrees * (M_PI / 180.0f);
	}

	[[nodiscard]]
	constexpr float ToDegree(float radians)
	{
		return radians * (180.0f / M_PI);
	}

	template<typename T> [[nodiscard]]
	constexpr T Abs(const T& val)
	{
		return std::abs(val);
	}

	template<typename T> [[nodiscard]]
	constexpr bool Equals(const T& a, const T& b, T epsilon = std::numeric_limits<std::decay_t<T>>::epsilon())
	{
		return Abs(a - b) <= epsilon;
	}

	// Merges two hash values non-symmetrically.
	[[nodiscard]]
	constexpr unsigned CombineHashes(unsigned a, unsigned b)
	{
		return a ^ (b + 0x9E3779B9 + (a << 6) + (a >> 2));
	}

	// Returns the lesser of all arguments.
	template<typename T, typename... Args> [[nodiscard]]
	constexpr T Min(const T& val1, const T& val2, const Args&... args)
	{
		if constexpr (sizeof...(Args) == 0)
		{
			if (val2 > val1)
				return val1;
			else
				return val2;
		}
		else
		{
			if (val2 > val1)
				return Min(val1, args...);
			else
				return Min(val2, args...);
		}
	}

	// Returns the greater of all arguments.
	template<typename T, typename... Args> [[nodiscard]]
	constexpr T Max(const T& val1, const T& val2, const Args&... args)
	{
		if constexpr (sizeof...(Args) == 0)
		{
			if (val1 < val2)
				return val2;
			else
				return val1;
		}
		else
		{
			if (val1 < val2)
				return Max(val2, args...);
			else
				return Max(val1, args...);
		}
	}

	// Clamps data to the range [low, high]
	template<typename T> [[nodiscard]]
	constexpr T Clamp(const T& data, const T& low, const T& high)
	{
		return (data < low) ? low : (data < high) ? data : high;
	}

	template<typename T> [[nodiscard]]
	constexpr T SmoothStep(const T& edge0, const T& edge1, float val)
	{
		val = Clamp((val - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return val * val * (3.0f - 2.0f * val);
	}

	template<typename T> [[nodiscard]]
	constexpr T SmootherStep(const T& edge0, const T& edge1, float val)
	{
		val = Clamp((val - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return val * val * val * (val * (val * 6.0f - 15.0f) + 10.0f);
	}

	template<typename T> [[nodiscard]]
	constexpr T Lerp(const T& data1, const T& data2, float percent)
	{
		return data1 * (1.0f - percent) + data2 * percent;
	}

	template<typename T> [[nodiscard]]
	constexpr T CatMull(const T& data1, const T& data2, const T& data3, const T& data4, float percent)
	{
		return 0.5f * (percent * (percent * (percent * (-data1 + 3.0f * data2 - 3.0f * data3 + data4) +
			(2.0f * data1 - 5.0f * data2 + 4.0f * data3 - data4)) +
			(-data1 + data3)) +
			2.0f * data2);
	}

	template<typename T> [[nodiscard]]
	constexpr T Bezier(const T& data1, const T& handle1, const T& handle2, const T& data2, float percent)
	{
		return percent * (percent * (percent * (-data1 + 3.0f * handle1 - 3.0f * handle2 + data2) +
			(3.0f * data1 - 6.0f * handle1 + 3.0f * handle2 + data2)) +
			(-3.0f * data1 + 3.0f * handle1)) +
			data1;
	}

	// Returns the next highest power of 2, or the same value if already a power of 2.
	// Returns 1 if the value is 0.
	[[nodiscard]]
	constexpr unsigned NextPowerOfTwo(unsigned value)
	{
		if (value == 0)
			return 1;

		value--;
		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;
		value++;

		return value;
	}

	// Returns the closest multiple of step.
	[[nodiscard]]
	float SnapToGrid(float value, float step);

	// Apply ease-in to a value in the range of [0, 1]
	[[nodiscard]]
	float EaseIn(float percent);

	// Apply ease-out to a value in the range of [0, 1]
	[[nodiscard]]
	float EaseOut(float percent);

	// Apply ease-in and ease-out to a value in the range of [0, 1]
	[[nodiscard]]
	constexpr float EaseInOut(float percent)
	{
		return SmoothStep(0.0f, 1.0f, percent);
	}
}
