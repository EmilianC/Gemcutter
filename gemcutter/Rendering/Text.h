// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Rendering/Renderable.h"
#include "gemcutter/Resource/Font.h"

#include <string>

namespace gem
{
	// Causes text to render at the entity's position.
	class Text : public Renderable
	{
	public:
		Text(Entity& owner);
		Text(Entity& owner, Font::Ptr font);
		Text(Entity& owner, std::string string);
		Text(Entity& owner, Material::Ptr material);
		Text(Entity& owner, Font::Ptr font, std::string string, Material::Ptr material);

		unsigned GetNumLines() const;
		float GetLineWidth(unsigned line) const;

		Font::Ptr font;
		std::string string;
		bool centeredX = false;
		bool centeredY = false;
		// Extra spacing between letters.
		float kerning = 0.0f;
	};
}
