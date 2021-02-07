// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <memory>

namespace gem
{
	template<class Derived>
	class Shareable : public std::enable_shared_from_this<Derived>
	{
		// Hide these functions from user code.
		using std::enable_shared_from_this<Derived>::shared_from_this;
		using std::enable_shared_from_this<Derived>::weak_from_this;

	protected:
		// A helper allowing private constructors to participate in the Shareable pattern.
		struct ShareableAlloc : public Derived
		{
			template<typename... Args>
			ShareableAlloc(Args&&... args) : Derived(std::forward<Args>(args)...) {}
		};

	public:
		using Ptr = std::shared_ptr<Derived>;
		using ConstPtr = std::shared_ptr<const Derived>;
		using WeakPtr = std::weak_ptr<Derived>;
		using ConstWeakPtr = std::weak_ptr<const Derived>;

		Ptr GetPtr()
		{
			return shared_from_this();
		}

		ConstPtr GetPtr() const
		{
			return shared_from_this();
		}

		WeakPtr GetWeakPtr()
		{
			return weak_from_this();
		}

		ConstWeakPtr GetWeakPtr() const
		{
			return weak_from_this();
		}

		template<typename... Args>
		static Ptr MakeNew(Args&&... params)
		{
			// If you have a compile error because the Derived class has an inaccessable private constructor,
			// declare "friend ShareableAlloc;" and MakeNew() will still be able to instantiate it.
			return std::make_shared<ShareableAlloc>(std::forward<Args>(params)...);
		}
	};
}
