// Copyright (c) 2021 Emilian Cioca
#pragma once
#include "gemcutter/GUI/Widget.h"
#include "gemcutter/Resource/Font.h"
#include "gemcutter/Resource/Material.h"

#include <string_view>
#include <string>

namespace gem
{
	class Text;

	// A widget containing text scaled to fit within it.
	class Label : public Widget
	{
	public:
		Label(Entity& owner);
		Label(Entity& owner, Font::Ptr font, std::string_view string);
		Label(Entity& owner, Font::Ptr font, std::string_view string, Material::Ptr material);

		Text& GetText();
		const Text& GetText() const;

		void SetString(std::string_view string);
		const std::string& GetString() const;

		void SetFont(Font::Ptr font);
		Font::Ptr GetFont() const;

		void SetMaterial(Material::Ptr material);
		Material::Ptr GetMaterial() const;

		float textScale = 1.0f;

	private:
		void UpdateContent() override;

		float textWidth = 0.0f;
		Text* text = nullptr;
	};
}
