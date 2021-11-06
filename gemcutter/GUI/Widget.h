// Copyright (c) 2021 Emilian Cioca
#pragma once
#include "gemcutter/Entity/Entity.h"
#include "gemcutter/Entity/Hierarchy.h"
#include "gemcutter/GUI/Rectangle.h"

namespace gem
{
	struct Edge
	{
		// Percentage of total distance from a parent edge.
		float anchor = 0.0f;
		// Flat offset from the anchor.
		float offset = 0.0f;
	};

	// A generic UI element sized with respect to its parent widget.
	class Widget : public Component<Widget>
	{
	public:
		Widget(Entity& owner);

		// Creates a new child widget of the given type.
		template<class T, typename... Args>
		T& CreateChild(Args&&... constructorParams);

		// Helper function which sets anchors to maintain the target size and position.
		// The given position should be relative to the bottom-left corner of the parent widget.
		// The parent widget's layout must be up to date.
		void SetTransform(vec2 position, vec2 size);

		// Sets the anchors to the full space available in the parent widget.
		// This is the default behaviour.
		void FitToParent();

		// Returns the current world-space dimensions of the widget.
		const Rectangle& GetAbsoluteBounds() const;

		// Called when the bounds of the widget are updated.
		// This is where specific widgets can adjust their content.
		virtual void UpdateContent() {}

		static void UpdateAll();

		Edge top;
		Edge bottom;
		Edge left;
		Edge right;

	private:
		void UpdateBounds(const Widget& parent);

		Rectangle absoluteBounds;
	};
}

#include "Widget.inl"
