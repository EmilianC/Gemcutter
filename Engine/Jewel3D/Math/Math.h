// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <cmath>

namespace Jwl
{
	constexpr float M_PI     = 3.14159265358979323846f;
	constexpr float M_E      = 2.71828182845904523536f;
	constexpr float M_LOG2E  = 1.44269504088896340736f;
	constexpr float M_LOG10E = 0.434294481903251827651f;
	constexpr float M_LN2    = 0.693147180559945309417f;
	constexpr float M_LN10   = 2.30258509299404568402f;

	constexpr float ToRadian(float degrees)
	{
		return degrees * (M_PI / 180.0f);
	}

	constexpr float ToDegree(float radians)
	{
		return radians * (180.0f / M_PI);
	}

	template<typename T>
	auto Abs(T val)
	{
		return std::abs(val);
	}

	template<typename T>
	bool Equals(T a, T b)
	{
		return Abs(a - b) <= std::numeric_limits<std::decay_t<T>>::epsilon();
	}

	// Merges two hash values non-symmetrically.
	unsigned CombineHashes(unsigned a, unsigned b);

	// Returns the lesser of all arguments.
	template<typename T0, typename T1, typename... Args>
	auto Min(T0&& val1, T1&& val2, Args&&... args)
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
				return Min(val1, std::forward<Args>(args)...);
			else
				return Min(val2, std::forward<Args>(args)...);
		}
	}

	// Returns the greater of all arguments.
	template<typename T0, typename T1, typename... Args>
	auto Max(T0&& val1, T1&& val2, Args&&... args)
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
				return Max(val2, std::forward<Args>(args)...);
			else
				return Max(val1, std::forward<Args>(args)...);
		}
	}

	// Clamps data to the range [low, high]
	template<typename T0, typename T1, typename T2>
	constexpr auto Clamp(T0&& data, T1&& low, T2&& high)
	{
		return Min(Max(data, low), high);
	}

	template<typename T0, typename T1, typename Common = std::common_type_t<T0, T1>>
	constexpr Common SmoothStep(T0&& edge0, T1&& edge1, Common val)
	{
		val = Clamp((val - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return val * val * (3.0f - 2.0f * val);
	}

	template<typename T0, typename T1, typename Common = std::common_type_t<T0, T1>>
	constexpr Common SmootherStep(T0& edge0, T1& edge1, Common val)
	{
		val = Clamp((val - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return val * val * val * (val * (val * 6.0f - 15.0f) + 10.0f);
	}

	template<typename T0, typename T1>
	constexpr auto Lerp(T0&& data1, T1&& data2, float percent)
	{
		return data1 * (1.0f - percent) + data2 * percent;
	}

	template<typename T0, typename T1, typename T2, typename T3>
	constexpr auto CatMull(T0&& data1, T1&& data2, T2&& data3, T3&& data4, float percent)
	{
		return 0.5f * (percent * (percent * (percent * (-data1 + 3.0f * data2 - 3.0f * data3 + data4) +
			(2.0f * data1 - 5.0f * data2 + 4.0f * data3 - data4)) +
			(-data1 + data3)) +
			2.0f * data2);
	}

	template<typename T0, typename T1, typename T2, typename T3>
	constexpr auto Bezier(T0&& data1, T1&& handle1, T2&& handle2, T3&& data2, float percent)
	{
		return percent * (percent * (percent * (-data1 + 3.0f * handle1 - 3.0f * handle2 + data2) +
			(3.0f * data1 - 6.0f * handle1 + 3.0f * handle2 + data2)) +
			(-3.0f * data1 + 3.0f * handle1)) +
			data1;
	}

	// Returns the closest multiple of step.
	float SnapToGrid(float value, float step);

	// Apply ease-in to a value in the range of [0, 1]
	float EaseIn(float percent);

	// Apply ease-out to a value in the range of [0, 1]
	float EaseOut(float percent);

	// Apply ease-in and ease-out to a value in the range of [0, 1]
	constexpr float EaseInOut(float percent)
	{
		return SmoothStep(0.0f, 1.0f, percent);
	}
}
