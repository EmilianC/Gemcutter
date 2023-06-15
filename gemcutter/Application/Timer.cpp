// Copyright (c) 2017 Emilian Cioca
#include "Timer.h"
#include "gemcutter/Application/Logging.h"

#include <Windows.h>

namespace
{
	int64_t GetTimerResolution()
	{
		LARGE_INTEGER temp;
		if (QueryPerformanceFrequency(&temp))
		{
			return temp.QuadPart;
		}

		gem::Error("Timer: High performance timer is not supported by your CPU.");
		return 0;
	}

	const int64_t ticksPerSecond = GetTimerResolution();
	const int64_t ticksPerMS = ticksPerSecond / 1000;

	// Casted ahead of time for a performance boost in GetElapsedMS() and GetElapsedSeconds().
	const double d_ticksPerSecond = static_cast<double>(ticksPerSecond);
	const double d_ticksPerMS = static_cast<double>(ticksPerMS);
}

namespace gem
{
	Timer::Timer()
	{
		Reset();
	}

	bool Timer::IsSupported()
	{
		return ticksPerSecond > 0;
	}

	int64_t Timer::GetTicksPerMS()
	{
		return ticksPerMS;
	}

	int64_t Timer::GetTicksPerSecond()
	{
		return ticksPerSecond;
	}

	int64_t Timer::GetCurrentTick()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		return currentTime.QuadPart;
	}

	void Timer::Reset()
	{
		startTime = GetCurrentTick();
	}

	double Timer::GetElapsedMS() const
	{
		return (GetCurrentTick() - startTime) / d_ticksPerMS;
	}

	double Timer::GetElapsedSeconds() const
	{
		return (GetCurrentTick() - startTime) / d_ticksPerSecond;
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
		startTime += static_cast<int64_t>(ms) / ticksPerMS;
	}

	void Timer::SubtractTimeSeconds(double seconds)
	{
		// Moving the start time forward effectively removes time.
		startTime += static_cast<int64_t>(seconds) / ticksPerSecond;
	}

	void Timer::AddTimeMS(double ms)
	{
		// Moving the start time back effective adds more time.
		startTime -= static_cast<int64_t>(ms) / ticksPerMS;
	}

	void Timer::AddTimeSeconds(double seconds)
	{
		// Moving the start time back effective adds more time.
		startTime -= static_cast<int64_t>(seconds) / ticksPerSecond;
	}
}

REFLECT(gem::Timer)
	MEMBERS {
		REF_PRIVATE_MEMBER(startTime)
	}
REF_END;
