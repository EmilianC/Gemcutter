// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Timer.h"
#include "Logging.h"

#include <Windows.h>

namespace
{
	__int64 GetTimerResolution()
	{
		LARGE_INTEGER temp;
		if (QueryPerformanceFrequency(&temp))
		{
			return temp.QuadPart;
		}

		Jwl::Error("Timer: High performance timer is not supported by your CPU.");
		return 0;
	}

	const __int64 ticksPerSecond = GetTimerResolution();
	const __int64 ticksPerMS = ticksPerSecond / 1000;

	//- Casted ahead of time for a performance boost in GetElapsedMS() and GetElapsedSeconds().
	const Jwl::f64 d_ticksPerSecond = static_cast<Jwl::f64 > (ticksPerSecond);
	const Jwl::f64 d_ticksPerMS = static_cast<Jwl::f64>(ticksPerMS);
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

	s64 Timer::GetTicksPerMS()
	{
		return ticksPerMS;
	}

	s64 Timer::GetTicksPerSecond()
	{
		return ticksPerSecond;
	}

	s64 Timer::GetCurrentTick()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		return currentTime.QuadPart;
	}

	void Timer::Reset()
	{
		startTime = GetCurrentTick();
	}

	f64 Timer::GetElapsedMS() const
	{
		return (GetCurrentTick() - startTime) / d_ticksPerMS;
	}

	f64 Timer::GetElapsedSeconds() const
	{
		return (GetCurrentTick() - startTime) / d_ticksPerSecond;
	}

	bool Timer::IsElapsedMS(f64 ms) const
	{
		return ms >= GetElapsedMS();
	}

	bool Timer::IsElapsedSeconds(f64 seconds) const
	{
		return seconds >= GetElapsedSeconds();
	}

	void Timer::SubtractTimeMS(f64 ms)
	{
		// Moving the start time forward effectively removes time.
		startTime += static_cast<__int64>(ms) / ticksPerMS;
	}

	void Timer::SubtractTimeSeconds(f64 seconds)
	{
		// Moving the start time forward effectively removes time.
		startTime += static_cast<__int64>(seconds) / ticksPerSecond;
	}

	void Timer::AddTimeMS(f64 ms)
	{
		// Moving the start time back effective adds more time.
		startTime -= static_cast<__int64>(ms) / ticksPerMS;
	}

	void Timer::AddTimeSeconds(f64 seconds)
	{
		// Moving the start time back effective adds more time.
		startTime -= static_cast<__int64>(seconds) / ticksPerSecond;
	}
}
