// Copyright (c) 2021 Emilian Cioca
#include "Widget.h"
#include "gemcutter/Entity/Hierarchy.h"

namespace gem
{
	Widget::Widget(Entity& _owner)
		: Component(_owner)
	{
		owner.Require<Hierarchy>().propagateTransform = false;
	}

	void Widget::SetTransform(vec2 position, vec2 size)
	{
		const float halfWidth  = size.x * -0.5f;
		const float halfHeight = size.y * -0.5f;

		top.offset    = halfHeight;
		bottom.offset = halfHeight;
		left.offset   = halfWidth;
		right.offset  = halfWidth;

		if (Entity::Ptr parent = owner.Get<Hierarchy>().GetParent())
		{
			const Rectangle& parentBounds = parent->Get<Widget>().GetAbsoluteBounds();

			const float percentageX = position.x / parentBounds.width;
			const float percentageY = position.y / parentBounds.height;

			top.anchor = (1.0f - percentageY);
			bottom.anchor = percentageY;
			left.anchor = percentageX;
			right.anchor = (1.0f - percentageX);
		}
		else
		{
			absoluteBounds.x = position.x;
			absoluteBounds.y = position.y;
		}
	}

	void Widget::FitToParent()
	{
		top.anchor = 0.0f;
		top.offset = 0.0f;

		bottom.anchor = 0.0f;
		bottom.offset = 0.0f;

		left.anchor = 0.0f;
		left.offset = 0.0f;

		right.anchor = 0.0f;
		right.offset = 0.0f;
	}

	const Rectangle& Widget::GetAbsoluteBounds() const
	{
		return absoluteBounds;
	}

	void Widget::UpdateAll()
	{
		for (Entity& entity : With<Widget, HierarchyRoot>())
		{
			auto& widget = entity.Get<Widget>();
			widget.absoluteBounds.width = widget.right.offset - widget.left.offset;
			widget.absoluteBounds.height = widget.top.offset - widget.bottom.offset;

			for (Entity::Ptr& child : entity.Get<Hierarchy>().GetChildren())
			{
				if (auto* childWidget = child->Try<Widget>())
				{
					childWidget->UpdateBounds(widget);
				}
			}
		}
	}

	void Widget::UpdateBounds(const Widget& parent)
	{
		const float offsetFromTop    = parent.absoluteBounds.height * top.anchor + top.offset;
		const float offsetFromBottom = parent.absoluteBounds.height * bottom.anchor + bottom.offset;
		const float offsetFromLeft   = parent.absoluteBounds.width * left.anchor + left.offset;
		const float offsetFromRight  = parent.absoluteBounds.width * right.anchor + right.offset;

		// These values are in world-space.
		const float topEdge    = (parent.absoluteBounds.y + parent.absoluteBounds.height) - offsetFromTop;
		const float bottomEdge = parent.absoluteBounds.y + offsetFromBottom;
		const float leftEdge   = parent.absoluteBounds.x + offsetFromLeft;
		const float rightEdge  = (parent.absoluteBounds.x + parent.absoluteBounds.width) - offsetFromRight;

		absoluteBounds.x = leftEdge;
		absoluteBounds.y = bottomEdge;
		absoluteBounds.width = rightEdge - leftEdge;
		absoluteBounds.height = topEdge - bottomEdge;

		// The actual position of the entity is the pivot (center of the widget).
		owner.position.x = absoluteBounds.x + absoluteBounds.width * 0.5f;
		owner.position.y = absoluteBounds.y + absoluteBounds.height * 0.5f;

		UpdateContent();

		for (Entity::Ptr& child : owner.Get<Hierarchy>().GetChildren())
		{
			if (auto* childWidget = child->Try<Widget>())
			{
				childWidget->UpdateBounds(*this);
			}
		}
	}
}

REFLECT(gem::Edge)
	MEMBERS {
		REF_MEMBER(anchor)
		REF_MEMBER(offset)
	}
REF_END;

REFLECT(gem::Widget) BASES { REF_BASE(gem::ComponentBase) }
	MEMBERS {
		REF_MEMBER(top)
		REF_MEMBER(bottom)
		REF_MEMBER(left)
		REF_MEMBER(right)
	}
REF_END;
