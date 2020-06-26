// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Rendering/Renderable.h"
#include "Jewel3D/Resource/Font.h"

#include <string>

namespace Jwl
{
	// Causes text to render at the entity's position.
	class Text : public Renderable
	{
	public:
		Text(Entity& owner);
		Text(Entity& owner, Font::Ptr font);
		Text(Entity& owner, std::string text);
		Text(Entity& owner, Material::Ptr material);
		Text(Entity& owner, Font::Ptr font, std::string text, Material::Ptr material);

		unsigned GetNumLines() const;
		float GetLineWidth(unsigned line) const;

		Font::Ptr font;
		std::string text;
		bool centeredX = false;
		bool centeredY = false;
		// Scale of spacing between letters.
		float kernel = 1.0f;
	};
}
