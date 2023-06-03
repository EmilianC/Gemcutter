// Copyright (c) 2021 Emilian Cioca
#include "Delegate.h"

namespace gem
{
	namespace detail
	{
		DelegateBase::DelegateBase()
			: controlBlock(std::make_shared<ControlBlock>(this))
		{
		}

		DelegateBase::DelegateBase(DelegateBase&& other) noexcept
			: controlBlock(std::move(other.controlBlock))
		{
			controlBlock->delegate = this;
		}

		DelegateBase& DelegateBase::operator=(DelegateBase&& other) noexcept
		{
			controlBlock = std::move(other.controlBlock);
			controlBlock->delegate = this;

			return *this;
		}
	}

	DelegateHandle::DelegateHandle(std::weak_ptr<detail::DelegateBase::ControlBlock> blockPtr, detail::DelegateId handleId)
		: controlBlock(std::move(blockPtr))
		, id(handleId)
	{
	}

	DelegateHandle::~DelegateHandle()
	{
		Expire();
	}

	void DelegateHandle::Expire()
	{
		if (auto lock = controlBlock.lock())
		{
			lock->delegate->Unbind(*this);
			controlBlock.reset();
		}
	}
}
