// Copyright (c) 2020 Emilian Cioca
#include "Hierarchy.h"

namespace gem
{
	Hierarchy::Hierarchy(Entity& _owner)
		: Component(_owner)
	{
		owner.Tag<HierarchyRoot>();
	}

	Hierarchy::~Hierarchy()
	{
		if (auto lock = parent.lock())
		{
			parentHierarchy->RemoveChild(owner);
		}
		else
		{
			owner.RemoveTag<HierarchyRoot>();
		}

		ClearChildren();
	}

	void Hierarchy::AddChild(Entity::Ptr entity)
	{
		ASSERT(entity, "Hierarchy cannot add null entity as child.");
		ASSERT(entity != owner, "Hierarchy cannot add itself as a child.");

		auto& childHierarchy = entity->Require<Hierarchy>();

		if (auto lock = childHierarchy.parent.lock())
		{
			childHierarchy.parentHierarchy->RemoveChild(*entity);
		}

		childHierarchy.parent = owner.GetWeakPtr();
		childHierarchy.parentHierarchy = this;
		entity->RemoveTag<HierarchyRoot>();
		children.push_back(std::move(entity));
	}

	void Hierarchy::RemoveChild(Entity& entity)
	{
		for (unsigned i = 0; i < children.size(); ++i)
		{
			if (children[i] == entity)
			{
				auto& childHierarchy = entity.Get<Hierarchy>();

				childHierarchy.parent.reset();
				childHierarchy.parentHierarchy = nullptr;
				entity.Tag<HierarchyRoot>();
				children.erase(children.begin() + i);

				return;
			}
		}
	}

	bool Hierarchy::IsChild(const Entity& child) const
	{
		auto lock = child.Get<Hierarchy>().parent.lock();
		return lock == owner;
	}

	bool Hierarchy::IsChildOf(const Entity& _parent) const
	{
		auto lock = parent.lock();
		return lock == _parent;
	}

	bool Hierarchy::IsDescendant(const Entity& descendant) const
	{
		auto* hierarchy = descendant.Get<Hierarchy>().GetParentHierarchy();
		while (hierarchy)
		{
			if (hierarchy == this)
			{
				return true;
			}

			hierarchy = hierarchy->GetParentHierarchy();
		}

		return false;
	}

	bool Hierarchy::IsDescendantOf(const Entity& ancestor) const
	{
		auto* hierarchy = GetParentHierarchy();
		while (hierarchy)
		{
			if (hierarchy->owner == ancestor)
			{
				return true;
			}

			hierarchy = hierarchy->GetParentHierarchy();
		}

		return false;
	}

	void Hierarchy::ClearChildren()
	{
		for (auto& child : children)
		{
			auto& childHierarchy = child->Get<Hierarchy>();

			childHierarchy.parent.reset();
			childHierarchy.parentHierarchy = nullptr;
			child->Tag<HierarchyRoot>();
		}

		children.clear();
	}

	void Hierarchy::DetachFromParent()
	{
		if (auto lock = parent.lock())
		{
			lock->Get<Hierarchy>().RemoveChild(owner);
		}
	}

	const Entity& Hierarchy::GetRoot() const
	{
		if (auto lock = parent.lock())
		{
			return parentHierarchy->GetRoot();
		}
		else
		{
			return owner;
		}
	}

	Entity& Hierarchy::GetRoot()
	{
		if (auto lock = parent.lock())
		{
			return parentHierarchy->GetRoot();
		}
		else
		{
			return owner;
		}
	}

	Entity::Ptr Hierarchy::GetParent() const
	{
		return parent.lock();
	}

	Hierarchy* Hierarchy::GetParentHierarchy() const
	{
		if (auto lock = parent.lock())
		{
			return &lock->Get<Hierarchy>();
		}

		return nullptr;
	}

	unsigned Hierarchy::GetNumChildren() const
	{
		return children.size();
	}

	unsigned Hierarchy::GetDepth() const
	{
		if (auto lock = parent.lock())
		{
			return parentHierarchy->GetDepth() + 1;
		}
		else
		{
			return 0;
		}
	}

	bool Hierarchy::IsRoot() const
	{
		return parent.expired();
	}

	bool Hierarchy::IsLeaf() const
	{
		return children.empty();
	}

	Entity::Ptr Hierarchy::CreateChild()
	{
		auto child = Entity::MakeNew();
		AddChild(child);

		return child;
	}

	Entity::Ptr Hierarchy::CreateChild(std::string name)
	{
		auto child = Entity::MakeNew(std::move(name));
		AddChild(child);

		return child;
	}

	mat4 Hierarchy::GetWorldTransform() const
	{
		mat4 transform(owner.rotation, owner.position, owner.scale);

		if (auto lock = parent.lock())
		{
			if (parentHierarchy->propagateTransform)
			{
				transform = parentHierarchy->GetWorldTransform() * transform;
			}
		}

		return transform;
	}

	quat Hierarchy::GetWorldRotation() const
	{
		quat result = owner.rotation;

		if (auto lock = parent.lock())
		{
			result = parentHierarchy->GetWorldRotation() * result;
		}

		return result;
	}
}

REFLECT_TAG(gem::HierarchyRoot);

REFLECT(gem::Hierarchy) BASES { REF_BASE(gem::ComponentBase) }
	MEMBERS {
		REF_MEMBER(propagateTransform)
		REF_PRIVATE_MEMBER(parentHierarchy, gem::ReadOnly)
		REF_PRIVATE_MEMBER(parent, gem::ReadOnly)
		REF_PRIVATE_MEMBER(children, gem::ReadOnly)
	}
REF_END;
