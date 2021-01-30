// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <Windows.h>

namespace Jwl
{
	class Mutex
	{
	public:
		~Mutex();

		bool Init();

		// Locks others out of the mutex.
		void Lock();

		// Releases the mutex for other threads to use.
		void Unlock();

	private:
		HANDLE mutex = 0;
	};

	class Thread
	{
	public:
		Thread() = default;
		Thread(const Thread&) = delete;
		~Thread();

		Thread& operator=(const Thread&) = delete;

		// Start the thread on the specified function. Returns false if thread didn't start.
		bool Start(unsigned (__stdcall *startFunc)(void* arg), void* argData = NULL);

		// Returns only once the internal thread has finished.
		void Join();

		// Returns false if the function could not retrieve the return value of the thread.
		bool GetReturnValue(unsigned* value);

		// Returns the thread ID.
		DWORD GetID() const;

	private:
		HANDLE threadHandle = 0;
		DWORD id = 0;
	};
}
