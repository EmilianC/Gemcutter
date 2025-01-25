// Copyright (c) 2020 Emilian Cioca
#pragma once
#include "gemcutter/Entity/Entity.h"
#include "gemcutter/Math/Matrix.h"
#include "gemcutter/Math/Quaternion.h"

#include <string>
#include <vector>

namespace gem
{
	// A Tag identifying an Entity with no parents.
	struct HierarchyRoot : public Tag<HierarchyRoot> {};

	// Allows Entities to be organized in a tree structure.
	// Can also propagate transformations from parent to child.
	class Hierarchy : public Component<Hierarchy>
	{
	public:
		Hierarchy(Entity& owner);
		~Hierarchy();

		// Attaches a child.
		void AddChild(Entity::Ptr entity);

		// Detaches the given child.
		void RemoveChild(Entity& entity);

		// Returns true if the given Entity is a direct child of this one.
		bool IsChild(const Entity& child) const;

		// Returns true if this Entity is a direct child of the given Entity.
		bool IsChildOf(const Entity& parent) const;

		// Returns true if the given Entity is a descendant of this one (at any depth).
		bool IsDescendant(const Entity& descendant) const;

		// Returns true if this Entity is a descendant of the given Entity (at any depth).
		bool IsDescendantOf(const Entity& ancestor) const;

		// Detaches all children.
		void ClearChildren();

		// Detaches from the parent, if we have one.
		void DetachFromParent();

		// Finds the root of the hierarchy, which could be this Entity.
		const Entity& GetRoot() const;

		// Finds the root of the hierarchy, which could be this Entity.
		Entity& GetRoot();

		// Returns the parent we are attached to, if we have one.
		Entity::Ptr GetParent() const;

		// Returns the parent's hierarchy component, if there is one.
		Hierarchy* GetParentHierarchy() const;

		// Returns the number of children currently held by this Entity.
		std::size_t GetNumChildren() const;

		// Gets the list of children held by this Entity.
		const auto& GetChildren() const { return children; }
		auto& GetChildren() { return children; }

		// Iterates over all direct children, and optionally all decedents.
		// Children should not be added or removed in during this function.
		template<class Self, class Functor>
		void ForEachChild(this Self& self, bool recursive, Functor&& Func);

		// Gets the depth of this Entity in the hierarchy. The root is always depth 0.
		// Direct children of the root are at depth 1, and so on.
		unsigned GetDepth() const;

		// Returns true if this Entity doesn't have a parent.
		bool IsRoot() const;

		// Returns true if this Entity doesn't have any children.
		bool IsLeaf() const;

		// Creates and returns a new child Entity.
		Entity::Ptr CreateChild();
		Entity::Ptr CreateChild(std::string name);

		// Returns the world-space transformation of the Entity, accumulated from the root of the hierarchy.
		mat4 GetWorldTransform() const;

		// Returns the world-space rotation of the Entity, accumulated from the root of the hierarchy.
		quat GetWorldRotation() const;

		// Whether or not this Entity propagates its transformations downwards through the hierarchy.
		bool propagateTransform = true;

	private:
		Hierarchy* parentHierarchy = nullptr;
		Entity::WeakPtr parent;
		std::vector<Entity::Ptr> children;

	public:
		PRIVATE_MEMBER(Hierarchy, parentHierarchy);
		PRIVATE_MEMBER(Hierarchy, parent);
		PRIVATE_MEMBER(Hierarchy, children);
	};
}

#include "Hierarchy.inl"
