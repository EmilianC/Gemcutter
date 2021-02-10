// Copyright (c) 2017 Emilian Cioca
#pragma once

/*
 ScopeGuard assists in preventing memory leaks and making code more clear.
 Following RAII principles, this can allow you to create safer functions.
 
 A function that allocates memory but has many exit points can easily leak
 unless you remember to duplicate your cleanup code in every return case. For example:
 
 void Load() {
 	int* memory = new int[256];
 	
 	if (!okay) return; // Leaks memory!
 	
 	// Many more exit points...
 	
 	delete[] int; // Only here will the memory be safely released.
 }
 
 With the scope guard we improve readability, safety, and maintainability.
 Write the cleanup code once, and it will run however the scope exits.
 
 void Load() {
 	int* memory = new int[256];
 	defer { delete[] memory };
 	
 	if (!okay) return; // defer block is called!
 	
 	// Many more (safe) exit points...
 	
 	// defer block is called. No need to manually delete.
 }
 
 Based on the implementation by Andrei Alexandrescu.
*/

namespace gem
{
	template<typename Functor>
	class ScopeGuard
	{
	public:
		ScopeGuard() = delete;
		ScopeGuard(const ScopeGuard&) = delete;
		ScopeGuard& operator=(const ScopeGuard&) = delete;

		ScopeGuard(ScopeGuard&& rhs) noexcept
			: functor(std::move(rhs.functor))
			, active(rhs.active)
		{
			rhs.Dismiss();
		}

		ScopeGuard(Functor func)
			: functor(std::move(func))
		{
		}

		~ScopeGuard()
		{
			if (active) functor();
		}

		// Executes the function if it hasn't already.
		// It will not be called again when the scope is terminated.
		void Execute()
		{
			if (active) functor();
			Dismiss();
		}

		// Stops the guard from activating when the scope is terminated.
		void Dismiss()
		{
			active = false;
		}

	private:
		Functor functor;
		bool active = true;
	};
}

#define CONCATENATE(s1, s2) s1##s2
#define CONCATENATE_INDIRECT(s1, s2) CONCATENATE(s1, s2)
#define ANONYMOUS_VARIABLE(str) CONCATENATE_INDIRECT(str, __COUNTER__)
#define defer gem::ScopeGuard ANONYMOUS_VARIABLE(SCOPE_GUARD_) = [&]()
