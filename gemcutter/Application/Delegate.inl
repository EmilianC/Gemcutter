// Copyright (c) 2021 Emilian Cioca
namespace gem
{
	template<typename Return, typename... Args>
	DelegateHandle Delegate<Return(Args...)>::Bind(std::function<Return(Args...)> functor)
	{
		ASSERT(functor, "'functor' is null.");

		lifetimePtr.reset();
		func = std::move(functor);
		id = GenerateUniqueId<detail::DelegateId>();

		return {controlBlock, id};
	}

	template<typename Return, typename... Args>
	void Delegate<Return(Args...)>::Bind(std::weak_ptr<void> lifetimeObject, std::function<Return(Args...)> functor)
	{
		ASSERT(functor, "'functor' is null.");
		ASSERT(!lifetimeObject.expired(), "'lifetimeObject' is already expired.");

		lifetimePtr = std::move(lifetimeObject);
		func = std::move(functor);
		id.Reset();
	}

	template<typename Return, typename... Args>
	void Delegate<Return(Args...)>::BindOwned(std::function<Return(Args...)> functor)
	{
		ASSERT(functor, "'functor' is null.");

		lifetimePtr.reset();
		func = std::move(functor);
		id = GenerateUniqueId<detail::DelegateId>();
	}

	template<typename Return, typename... Args>
	void Delegate<Return(Args...)>::Clear()
	{
		lifetimePtr.reset();
		func = nullptr;
		id.Reset();
	}

	template<typename Return, typename... Args>
	Delegate<Return(Args...)>::operator bool() const
	{
		return !!func;
	}

	template<typename Return, typename... Args>
	template<typename... Params>
	auto Delegate<Return(Args...)>::operator()(Params&&... params) -> std::conditional_t<HasReturnValue, std::optional<Return>, void>
	{
		if constexpr (HasReturnValue)
		{
			std::optional<Return> result;
			if (func)
			{
				if (IsPtrNull(lifetimePtr) || !lifetimePtr.expired())
				{
					result = func(std::forward<Params>(params)...);
				}
				else
				{
					Clear();
				}
			}

			return result;
		}
		else
		{
			if (func)
			{
				if (IsPtrNull(lifetimePtr) || !lifetimePtr.expired())
				{
					func(std::forward<Params>(params)...);
				}
				else
				{
					Clear();
				}
			}
		}
	}

	template<typename Return, typename... Args>
	void Delegate<Return(Args...)>::Unbind(const DelegateHandle& handle)
	{
		if (id == handle.id)
		{
			Clear();
		}
	}

	template<typename Return, typename... Args>
	DelegateHandle Dispatcher<Return(Args...)>::Add(std::function<Return(Args...)> functor)
	{
		ASSERT(functor, "'functor' is null.");
#ifdef GEM_DEBUG
		ASSERT(!dispatching,
			"Adding a new functor cannot be done safely because the Dispatcher is currently being invoked higher in the call stack.");
#endif

		const auto id = GenerateUniqueId<detail::DelegateId>();
		bindings.emplace_back(std::weak_ptr<void>{}, std::move(functor), id);

		return {controlBlock, id};
	}

	template<typename Return, typename... Args>
	void Dispatcher<Return(Args...)>::Add(std::weak_ptr<void> lifetimeObject, std::function<Return(Args...)> functor)
	{
		ASSERT(functor, "'functor' is null.");
		ASSERT(!lifetimeObject.expired(), "'lifetimeObject' is already expired.");
#ifdef GEM_DEBUG
		ASSERT(!dispatching,
			"Adding a new functor cannot be done safely because the Dispatcher is currently being invoked higher in the call stack.");
#endif

		bindings.emplace_back(std::move(lifetimeObject), std::move(functor), detail::DelegateId{});
	}

	template<typename Return, typename... Args>
	void Dispatcher<Return(Args...)>::AddOwned(std::function<Return(Args...)> functor)
	{
		ASSERT(functor, "'functor' is null.");
#ifdef GEM_DEBUG
		ASSERT(!dispatching,
			"Adding a new functor cannot be done safely because the Dispatcher is currently being invoked higher in the call stack.");
#endif

		const auto id = GenerateUniqueId<detail::DelegateId>();
		bindings.emplace_back(std::weak_ptr<void>{}, std::move(functor), id);
	}

	template<typename Return, typename... Args>
	void Dispatcher<Return(Args...)>::Clear()
	{
#ifdef GEM_DEBUG
		ASSERT(!dispatching,
			"Clear() cannot be executed safely because the Dispatcher is currently being invoked higher in the call stack.\n"
			"Consider calling Expire() on a DelegateHandle instead.");
#endif

		bindings.clear();
	}

	template<typename Return, typename... Args>
	Dispatcher<Return(Args...)>::operator bool() const
	{
		for (const Binding& binding : bindings)
		{
			if (binding.func)
				return true;
		}

		return false;
	}

	template<typename Return, typename... Args>
	template<typename... Params>
	void Dispatcher<Return(Args...)>::Dispatch(Params&&... params)
	{
#ifdef GEM_DEBUG
		ASSERT(!dispatching,
			"Call to Dispatch() cannot be executed because it is already being dispatched higher in the call stack.");
		dispatching = true;
#endif

		for (auto itr = std::begin(bindings); itr < std::end(bindings);)
		{
			Binding& binding = *itr;

			if (binding.func && (IsPtrNull(binding.lifetimePtr) || !binding.lifetimePtr.expired()))
			{
				binding.func(std::forward<Params>(params)...);
				++itr;
			}
			else
			{
				itr = bindings.erase(itr);
			}
		}

#ifdef GEM_DEBUG
		dispatching = false;
#endif
	}

	template<typename Return, typename... Args>
	void Dispatcher<Return(Args...)>::Unbind(const DelegateHandle& handle)
	{
		for (Binding& binding : bindings)
		{
			if (binding.id == handle.id)
			{
				// We do not erase the binding to avoid causing issues
				// if we are currently in the middle of dispatching.
				binding.lifetimePtr.reset();
				binding.func = nullptr;
				return;
			}
		}
	}
}
