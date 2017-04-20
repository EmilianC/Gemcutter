// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Timer.h"
#include "Logging.h"

#include <Windows.h>

namespace
{
	static __int64 ticksPerSecond = Jwl::Timer::GetResolution();
	static __int64 ticksPerMS = ticksPerSecond / 1000;

	//- Casted ahead of time for a performance boost in GetElapsedMS() and GetElapsedSeconds().
	static double d_ticksPerSecond = static_cast<double>(ticksPerSecond);
	static double d_ticksPerMS = static_cast<double>(ticksPerMS);
}

namespace Jwl
{
	Timer::Timer()
	{
		Reset();
	}

	bool Timer::IsSupported()
	{
		return ticksPerSecond > 0;
	}

	__int64 Timer::GetResolution()
	{
		LARGE_INTEGER temp;
		if (QueryPerformanceFrequency(&temp))
		{
			return temp.QuadPart;
		}

		Error("Timer: High performance timer is not supported by your CPU.");
		return 0;
	}

	void Timer::Reset()
	{
		LARGE_INTEGER temp;
		QueryPerformanceCounter(&temp);

		startTime = temp.QuadPart;
	}

	double Timer::GetElapsedMS() const
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		// (the total time the Timer has been running) / Conversion to MS
		return (currentTime.QuadPart - startTime) / d_ticksPerMS;
	}

	double Timer::GetElapsedSeconds() const
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		// (the total time the Timer has been running) / Conversion to seconds
		return (currentTime.QuadPart - startTime) / d_ticksPerSecond;
	}

	bool Timer::IsElapsedMS(double ms) const
	{
		return ms >= GetElapsedMS();
	}

	bool Timer::IsElapsedSeconds(double seconds) const
	{
		return seconds >= GetElapsedSeconds();
	}

	void Timer::SubtractTimeMS(double ms)
	{
		// Moving the start time forward effectively removes time.
		startTime += static_cast<__int64>(ms) / ticksPerMS;
	}

	void Timer::SubtractTimeSeconds(double seconds)
	{
		// Moving the start time forward effectively removes time.
		startTime += static_cast<__int64>(seconds) / ticksPerSecond;
	}

	void Timer::AddTimeMS(double ms)
	{
		// Moving the start time back effective adds more time.
		startTime -= static_cast<__int64>(ms) / ticksPerMS;
	}

	void Timer::AddTimeSeconds(double seconds)
	{
		// Moving the start time back effective adds more time.
		startTime -= static_cast<__int64>(seconds) / ticksPerSecond;
	}
}
