// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <cmath>

namespace Jwl
{
	constexpr float M_PI	 = 3.14159265358979323846f;
	constexpr float M_E		 = 2.71828182845904523536f;
	constexpr float M_LOG2E	 = 1.44269504088896340736f;
	constexpr float M_LOG10E = 0.434294481903251827651f;
	constexpr float M_LN2	 = 0.693147180559945309417f;
	constexpr float M_LN10	 = 2.30258509299404568402f;

	constexpr float ToRadian(float degrees)
	{
		return degrees * (M_PI / 180.0f);
	}

	constexpr float ToDegree(float radians)
	{
		return radians * (180.0f / M_PI);
	}

	template<typename T0>
	auto Abs(T0&& val)
	{
		return std::abs(val);
	}

	// Returns the lesser of all arguments.
	template<typename T0, typename T1, typename... Args>
	auto Min(T0&& val1, T1&& val2, Args&&... args)
	{
		using namespace detail;
		if (val2 > val1)
			return Min(val1, std::forward<Args>(args)...);
		else
			return Min(val2, std::forward<Args>(args)...);
	}

	// Returns the greater of all arguments.
	template<typename T0, typename T1, typename... Args>
	auto Max(T0&& val1, T1&& val2, Args&&... args)
	{
		using namespace detail;
		if (val1 < val2)
			return Max(val2, std::forward<Args>(args)...);
		else
			return Max(val1, std::forward<Args>(args)...);
	}

	// Clamps data to the range [low, high]
	template<class T>
	constexpr T Clamp(const T& data, const T& low, const T& high)
	{
		return Min(Max(data, low), high);
	}

	template<class T>
	constexpr T SmoothStep(const T& edge0, const T& edge1, T val)
	{
		val = Clamp((val - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return val * val * (3.0f - 2.0f * val);
	}

	template<class T>
	constexpr T SmootherStep(const T& edge0, const T& edge1, T val)
	{
		val = Clamp((val - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return val * val * val * (val * (val * 6.0f - 15.0f) + 10.0f);
	}

	template<class T>
	constexpr T Lerp(const T& data1, const T& data2, float percent)
	{
		return data1 * (1.0f - percent) + data2 * percent;
	}

	template<class T>
	constexpr T CatMull(const T& data1, const T& data2, const T& data3, const T& data4, float percent)
	{
		return 0.5f * (percent * (percent * (percent * (-data1 + 3.0f * data2 - 3.0f * data3 + data4) +
			(2.0f * data1 - 5.0f * data2 + 4.0f * data3 - data4)) +
			(-data1 + data3)) +
			2.0f * data2);
	}

	template<class T>
	constexpr T Bezier(const T& data1, const T& handle1, const T& handle2, const T& data2, float percent)
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

	namespace detail
	{
		template<typename T>
		T&& Min(T&& val)
		{
			return std::forward<T>(val);
		}

		template<typename T>
		T&& Max(T&& val)
		{
			return std::forward<T>(val);
		}
	}
}
