// Copyright (c) 2021 Emilian Cioca
#pragma once
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Utilities/Identifier.h"
#include "gemcutter/Utilities/StdExt.h"

#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

namespace gem
{
	class DelegateHandle;
	template<typename T> class Delegate   { static_assert(std::is_function_v<T>, "Must be given a function signature."); };
	template<typename T> class Dispatcher { static_assert(std::is_function_v<T>, "Must be given a function signature."); };

	namespace detail
	{
		struct DelegateId : public Identifier<unsigned> {};

		// Enables DelegateHandles to safely access their associated Delegate/Dispatcher.
		class DelegateBase
		{
			DelegateBase(const DelegateBase&) = delete;
			DelegateBase& operator=(const DelegateBase&) = delete;
		public:
			DelegateBase();
			DelegateBase(DelegateBase&&) noexcept;
			DelegateBase& operator=(DelegateBase&&) noexcept;

			virtual ~DelegateBase() = default;
			virtual void Unbind(const DelegateHandle&) = 0;

			struct ControlBlock
			{
				DelegateBase* delegate;
			};

		protected:
			std::shared_ptr<ControlBlock> controlBlock;
		};
	}

	// Enables control over when a delegate binding is expired.
	class [[nodiscard]] DelegateHandle
	{
		template<class T> friend class Delegate;
		template<class T> friend class Dispatcher;

		DelegateHandle(const DelegateHandle&) = delete;
		DelegateHandle& operator=(const DelegateHandle&) = delete;
		DelegateHandle(std::weak_ptr<detail::DelegateBase::ControlBlock> blockPtr, detail::DelegateId handleId);
	public:
		DelegateHandle(DelegateHandle&&) = default;
		DelegateHandle& operator=(DelegateHandle&&) = default;
		~DelegateHandle();

		// Disconnects the associated delegate binding.
		void Expire();

	private:
		std::weak_ptr<detail::DelegateBase::ControlBlock> controlBlock;
		detail::DelegateId id;
	};

	// Represents a safe binding to a single functor.
	template<typename Return, typename... Args>
	class Delegate<Return(Args...)> : public detail::DelegateBase
	{
	public:
		constexpr static bool HasReturnValue = !std::is_void_v<Return>;

		// Binds the lifetime of the functor to the returned a handle.
		DelegateHandle Bind(std::function<Return(Args...)> functor);
		// Binds the lifetime of the functor to the specified memory-managed object.
		void Bind(std::weak_ptr<void> lifetimeObject, std::function<Return(Args...)> functor);
		// Binds the lifetime of the functor to the delegate itself. This should only be used when the
		// lifetime of the callback would otherwise be bound to the same object that owns the delegate.
		void BindOwned(std::function<Return(Args...)> functor);

		void Clear();

		// Checks if a functor is bound, but does not verify that its lifetime is valid.
		operator bool() const;

		// Internally checks if the bound functor still has a valid lifetime.
		// An empty optional<> is returned if a return type exists, but the functor could not be invoked.
		template<typename... Params>
		auto operator()(Params&&... params) -> std::conditional_t<HasReturnValue, std::optional<Return>, void>;

	private:
		void Unbind(const DelegateHandle& handle) override;

		std::weak_ptr<void> lifetimePtr;
		std::function<Return(Args...)> func;
		detail::DelegateId id;
	};

	// A Delegate which supports multiple bound functors.
	// Return values are allowed but ignored.
	template<typename Return, typename... Args>
	class Dispatcher<Return(Args...)> : public detail::DelegateBase
	{
	public:
		// Binds the lifetime of the functor to the returned a handle.
		DelegateHandle Add(std::function<Return(Args...)> functor);
		// Binds the lifetime of the functor to the specified memory-managed object.
		void Add(std::weak_ptr<void> lifetimeObject, std::function<Return(Args...)> functor);
		// Binds the lifetime of the functor to the dispatcher itself. This should only be used when the
		// lifetime of the callback would otherwise be bound to the same object that owns the dispatcher.
		void AddOwned(std::function<Return(Args...)> functor);

		void Clear();

		// Checks if at least one functor is bound, but does not verify that its lifetime is valid.
		operator bool() const;

		// Invokes all the bound and valid functors.
		// Functors cannot be added or removed during the dispatch, except through a DelegateHandle's Expire().
		template<typename... Params>
		void Dispatch(Params&&... params);

	private:
		void Unbind(const DelegateHandle& handle) override;

		struct Binding
		{
			std::weak_ptr<void> lifetimePtr;
			std::function<Return(Args...)> func;
			detail::DelegateId id;
		};

		std::vector<Binding> bindings;

#ifdef GEM_DEBUG
		bool dispatching = false;
#endif
	};

	Delegate()   -> Delegate<void()>;
	Dispatcher() -> Dispatcher<void()>;
}

#include "Delegate.inl"
