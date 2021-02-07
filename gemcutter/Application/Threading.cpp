// Copyright (c) 2017 Emilian Cioca
#include "Threading.h"

namespace Jwl
{
	Mutex::~Mutex()
	{
		if (mutex != NULL)
		{
			CloseHandle(mutex);
		}
	}

	bool Mutex::Init()
	{
		mutex = CreateMutex(
			NULL,  // Default security
			FALSE, // Not initially owned
			NULL); // Unnamed

		return mutex != NULL;
	}

	void Mutex::Lock()
	{
		WaitForSingleObject(mutex, INFINITE);
	}

	void Mutex::Unlock()
	{
		ReleaseMutex(mutex);
	}

	Thread::~Thread()
	{
		if (threadHandle != NULL)
		{
			CloseHandle(threadHandle);
		}
	}

	bool Thread::Start(unsigned (__stdcall *startFunc)(void* arg), void* argData)
	{
		threadHandle = CreateThread(
			NULL,                                                // Default security
			NULL,                                                // Default stack size
			reinterpret_cast<LPTHREAD_START_ROUTINE>(startFunc), // Start location
			argData,                                             // Data argument
			NULL,                                                // Default creation
			&id);                                                // Thread ID

		return threadHandle != NULL;
	}

	void Thread::Join()
	{
		WaitForSingleObject(threadHandle, INFINITE);
	}

	bool Thread::GetReturnValue(unsigned* value)
	{
		bool result = GetExitCodeThread(threadHandle, reinterpret_cast<LPDWORD>(value)) != 0;
		return result && reinterpret_cast<DWORD>(value) != STILL_ACTIVE;
	}

	DWORD Thread::GetID() const
	{
		return id;
	}
}
