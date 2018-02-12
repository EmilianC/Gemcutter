// Copyright (c) 2017 Emilian Cioca
namespace Jwl
{
	template<class Node>
	Hierarchy<Node>& Hierarchy<Node>::operator=(const Hierarchy& other)
	{
		if (auto lock = other.parent.lock())
		{
			lock->AddChild(GetPtr());
		}
		else
		{
			parent.reset();
		}

		children.clear();

		return *this;
	}

	template<class Node>
	Hierarchy<Node>::~Hierarchy()
	{
		if (auto lock = parent.lock())
		{
			lock->RemoveChild(*static_cast<Node*>(this));
		}

		ClearChildren();
	}

	template<class Node>
	void Hierarchy<Node>::AddChild(typename Hierarchy<Node>::Ptr child)
	{
		ASSERT(child, "Hierarchy cannot add null node as child.");
		ASSERT(this != child.get(), "Hierarchy cannot add itself as a child.");

		if (auto lock = child->parent.lock())
		{
			lock->RemoveChild(*child);
		}

		child->parent = GetWeakPtr();
		children.push_back(child);
	}

	template<class Node>
	void Hierarchy<Node>::RemoveChild(Node& child)
	{
		for (unsigned i = 0; i < children.size(); ++i)
		{
			if (children[i].get() == &child)
			{
				child.parent.reset();
				children.erase(children.begin() + i);

				return;
			}
		}
	}

	template<class Node>
	bool Hierarchy<Node>::IsChild(const Node& node) const
	{
		auto lock = node.parent.lock();
		return lock && lock.get() == static_cast<const Node*>(this);
	}

	template<class Node>
	void Hierarchy<Node>::ClearChildren()
	{
		for (auto child : children)
		{
			child->parent.reset();
		}

		children.clear();
	}

	template<class Node>
	typename Hierarchy<Node>::ConstPtr Hierarchy<Node>::GetRoot() const
	{
		if (auto lock = parent.lock())
		{
			return lock->GetRoot();
		}
		else
		{
			return GetPtr();
		}
	}

	template<class Node>
	typename Hierarchy<Node>::Ptr Hierarchy<Node>::GetRoot()
	{
		if (auto lock = parent.lock())
		{
			return lock->GetRoot();
		}
		else
		{
			return GetPtr();
		}
	}

	template<class Node>
	typename Hierarchy<Node>::Ptr Hierarchy<Node>::GetParent() const
	{
		return parent.lock();
	}

	template<class Node>
	unsigned Hierarchy<Node>::GetNumChildren() const
	{
		return children.size();
	}

	template<class Node>
	unsigned Hierarchy<Node>::GetDepth() const
	{
		if (auto lock = parent.lock())
		{
			return lock->GetDepth() + 1;
		}
		else
		{
			return 0;
		}
	}

	template<class Node>
	bool Hierarchy<Node>::IsRoot() const
	{
		return parent.expired();
	}

	template<class Node>
	bool Hierarchy<Node>::IsLeaf() const
	{
		return children.empty();
	}
}
