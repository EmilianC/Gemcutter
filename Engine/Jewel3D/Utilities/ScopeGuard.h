// Copyright (c) 2017 Emilian Cioca
#pragma once

/*
* ScopeGuard assists in preventing memory leaks and makes coding easier.
* Following RAII principles, this can allow you to create safer functions.
* 
* A function that allocates memory but has many exit points can easily 
* leak memory. Unless you remember to duplicate your cleanup code in 
* every return case, you will leak memory. For example:
*
* void Load() {
*	int* memory = new int[256];
*	
*	if (!okay) return; // Leaks memory!
*	
*	// Many more exit points...
*	
*	delete[] int; // Only here will the data be released.
* }
* 
* With the scope guard we improve readability, safety, and maintainability.
* Write the cleanup code once, and it will run however the function exits.
*
* void Load() {
*	int* memory = new int[256];
*	defer { delete[] memory };
*
*	if (!okay) return; // defer block is called!
*
*	// Many more (safe) exit points...
*
*	// defer block is called. no need to manually delete.
* }
*
* Based on the implementation by Andrei Alexandrescu
*/

namespace Jwl
{
	template<typename LambdaFunc>
	class ScopeGuard
	{
	public:
		ScopeGuard() = delete;
		ScopeGuard(const ScopeGuard&) = delete;
		ScopeGuard& operator=(const ScopeGuard&) = delete;

		ScopeGuard(ScopeGuard&& rhs)
			: func(std::move(rhs.func))
			, active(rhs.active)
		{
			rhs.Disable();
		}
		
		ScopeGuard(LambdaFunc func)
			: func(std::move(func))
		{
		}

		~ScopeGuard()
		{
			if (active) func();
		}

		// Executes the function early. It will not be called again when
		// the scope is terminated.
		void Execute()
		{
			func();
			Disable();
		}

		// Stops the guard from activating when the scope is terminated.
		void Disable()
		{
			active = false;
		}

	private:
		LambdaFunc func;
		bool active = true;
	};

	template<typename T>
	ScopeGuard<T> MakeScopeGaurd(T func)
	{
		return ScopeGuard<T>(std::move(func));
	}

	enum class ScopeGuard_TYPE {};

	template<typename func>
	ScopeGuard<func> operator+(ScopeGuard_TYPE, func&& f)
	{
		return ScopeGuard<func>(std::forward<func>(f));
	}
}

#define CONCATENATE(s1, s2) s1##s2
#define CONCATENATE_INDIRECT(s1, s2) CONCATENATE(s1, s2)
#define ANONYMOUS_VARIABLE(str) CONCATENATE_INDIRECT(str, __COUNTER__)
#define defer auto ANONYMOUS_VARIABLE(SCOPE_GUARD_) = Jwl::ScopeGuard_TYPE() + [&]()
