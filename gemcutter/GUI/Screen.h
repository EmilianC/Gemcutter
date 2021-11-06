// Copyright (c) 2021 Emilian Cioca
#pragma once
#include "gemcutter/Application/Application.h"
#include "gemcutter/Application/Event.h"
#include "gemcutter/GUI/Widget.h"

namespace gem
{
	// A root UI widget which automatically resizes to the size of the application's screen.
	// This is intended to be the root widget for a standard UI setup.
	class Screen : public Widget
	{
	public:
		Screen(Entity& owner);

	private:
		// These inherited functions are hidden because they are not appropriate for a screen root.
		void SetTransform(vec2 position, vec2 size) = delete;
		void FitToParent() = delete;

		Listener<Resize> onResize;
	};
}
