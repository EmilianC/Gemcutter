// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Resource/Font.h"

#include <string>

namespace Jwl
{
	class Text : public Component<Text>
	{
	public:
		Text(Entity& owner);
		Text(Entity& owner, Font::Ptr font);
		Text(Entity& owner, const std::string& text);
		Text(Entity& owner, const std::string& text, Font::Ptr font);

		void SetFont(Font::Ptr font);
		Font::Ptr GetFont() const;
		unsigned GetNumLines() const;
		float GetLineWidth(unsigned line) const;

		std::string text;
		bool centeredX = false;
		bool centeredY = false;
		float kernel = 1.0f;

	private:
		Font::Ptr data;
	};
}
