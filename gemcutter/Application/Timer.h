// Copyright (c) 2017 Emilian Cioca
#pragma once

#include <cstdint>
#include <loupe/private_members.h>

namespace gem
{
	class Timer
	{
	public:
		// Calls Reset(), immediately starting the internal clock.
		Timer();

		// Returns true if performance counters are available on the system architecture.
		static bool IsSupported();

		// Returns the resolution of the timer in ticks-per-millisecond.
		static int64_t GetTicksPerMS();
		// Returns the resolution of the timer in ticks-per-second.
		static int64_t GetTicksPerSecond();

		// Returns the current global tick count. Can be subtracted from a previous call
		// in order to calculate the amount of time that has passed.
		static int64_t GetCurrentTick();

		// Sets the reference time for any other functions called in the future.
		void Reset();

		// Returns the amount of time passed since the last Reset() in milliseconds.
		double GetElapsedMS() const;
		// Returns the amount of time passed since the last Reset() in seconds.
		double GetElapsedSeconds() const;

		// Returns true if the specified amount of milliseconds have elapsed since the last Reset().
		bool IsElapsedMS(double ms) const;
		// Returns true if the specified amount of seconds have elapsed since the last Reset().
		bool IsElapsedSeconds(double seconds) const;

		// Removes milliseconds from the running internal clock.
		void SubtractTimeMS(double ms);
		// Removes seconds from the running internal clock.
		void SubtractTimeSeconds(double seconds);

		// Adds milliseconds to the running internal clock.
		void AddTimeMS(double ms);
		// Adds seconds to the running internal clock.
		void AddTimeSeconds(double seconds);

	private:
		int64_t startTime;

	public:
		PRIVATE_MEMBER(Timer, startTime);
	};
}
