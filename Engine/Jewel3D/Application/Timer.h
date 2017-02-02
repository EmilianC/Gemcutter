// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <Windows.h>

namespace Jwl
{
	class Timer
	{
	public:
		//- Calls Reset(), immediately starting the internal clock.
		Timer();

		//- Returns true if performance counters are available on the system architecture.
		static bool IsSupported();

		//- Sets the reference time for any other functions called in the future.
		void Reset();

		//- Returns the amount of time passed since the last Reset() in milliseconds.
		double GetElapsedMS() const;
		//- Returns the amount of time passed since the last Reset() in seconds.
		double GetElapsedSeconds() const;

		//- Returns true if the specified amount of milliseconds have elapsed since the last Reset().
		bool IsElapsedMS(double ms) const;
		//- Returns true if the specified amount of seconds have elapsed since the last Reset().
		bool IsElapsedSeconds(double seconds) const;

		//- Removes the milliseconds from the running internal clock.
		void SubtractTimeMS(double ms);
		//- Removes the seconds from the running internal clock.
		void SubtractTimeSeconds(double seconds);

		//- Adds the milliseconds to the running internal clock.
		void AddTimeMS(double ms);
		//- Adds the seconds to the running internal clock.
		void AddTimeSeconds(double seconds);

	private:
		static __int64 GetResolution();

		static __int64 ticksPerSecond;
		static __int64 ticksPerMS;

		LARGE_INTEGER startTime;
	};
}
