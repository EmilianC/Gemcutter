// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace Jwl
{
	class Timer
	{
	public:
		//- Calls Reset(), immediately starting the internal clock.
		Timer();

		//- Returns true if performance counters are available on the system architecture.
		static bool IsSupported();

		//- Returns the resolution of the timer in ticks-per-millisecond.
		static s64 GetTicksPerMS();
		//- Returns the resolution of the timer in ticks-per-second.
		static s64 GetTicksPerSecond();

		//- Returns the current global tick count. Can be subtracted from a previous call
		// in order to calculate the amount of time that has passed.
		static s64 GetCurrentTick();

		//- Sets the reference time for any other functions called in the future.
		void Reset();

		//- Returns the amount of time passed since the last Reset() in milliseconds.
		f64 GetElapsedMS() const;
		//- Returns the amount of time passed since the last Reset() in seconds.
		f64 GetElapsedSeconds() const;

		//- Returns true if the specified amount of milliseconds have elapsed since the last Reset().
		bool IsElapsedMS(f64 ms) const;
		//- Returns true if the specified amount of seconds have elapsed since the last Reset().
		bool IsElapsedSeconds(f64 seconds) const;

		//- Removes milliseconds from the running internal clock.
		void SubtractTimeMS(f64 ms);
		//- Removes seconds from the running internal clock.
		void SubtractTimeSeconds(f64 seconds);

		//- Adds milliseconds to the running internal clock.
		void AddTimeMS(f64 ms);
		//- Adds seconds to the running internal clock.
		void AddTimeSeconds(f64 seconds);

	private:
		s64 startTime;
	};
}
