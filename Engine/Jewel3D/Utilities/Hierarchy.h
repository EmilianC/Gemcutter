// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Resource/Shareable.h"

#include <vector>

namespace Jwl
{
	//- Manages a hierarchy of a generic types.
	//- All nodes are managed by smart pointers.
	//- Derive from this to create a hierarchical type. Your class should pass itself as the template:
	//	class NodeData : public Hierarchy<NodeData> { /* */ };
	template<class Node>
	class Hierarchy : public Shareable<Node>
	{
	public:
		Hierarchy& operator=(const Hierarchy&);
		~Hierarchy();

		//- Appends a child to this node.
		void AddChild(Ptr child);

		//- Removes the child from the hierarchy,
		void RemoveChild(Node& child);

		//- Returns true if the specifed node is a child of this one.
		bool IsChild(const Node& node) const;

		//- Removes all children from this node.
		void ClearChildren();

		//- Finds the root node of the hierarchy, which could be this node.
		ConstPtr GetRoot() const;

		//- Finds the root node of the hierarchy, which could be this node.
		Ptr GetRoot();

		//- Returns this node's parent, if it has one.
		Ptr GetParent() const;

		//- Returns the number of children currently held by this node.
		unsigned GetNumChildren() const;

		//- Gets the list of children held by this node.
		const auto& GetChildren() const { return children; }

		//- Gets the depth of this node in the hierarchy. The root is always depth 0.
		//	Direct children of the root are at depth 1, and so on.
		unsigned GetDepth() const;

		//- Returns true if this node doesn't have a parent.
		bool IsRoot() const;

		//- Returns true if this node doesn't have any children.
		bool IsLeaf() const;

	private:
		WeakPtr parent;
		std::vector<Ptr> children;
	};
}

#include "Hierarchy.inl"
