// Copyright (c) 2021 Emilian Cioca
#include "Label.h"
#include "gemcutter/Rendering/Text.h"

namespace gem
{
	Label::Label(Entity& _owner)
		: Widget(_owner)
	{
		auto material = Material::MakeNew();
		material->shader = Load<Shader>("Shaders/Default/Font");
		material->blendMode = BlendFunc::Linear;

		text = &owner.Require<Text>();
		text->centeredX = true;
		text->centeredY = true;
		text->SetMaterial(std::move(material));
	}

	Label::Label(Entity& _owner, Font::Ptr font, std::string_view string)
		: Label(_owner)
	{
		SetFont(std::move(font));
		SetString(string);
	}

	Label::Label(Entity& _owner, Font::Ptr font, std::string_view string, Material::Ptr material)
		: Widget(_owner)
	{
		text = &owner.Require<Text>();
		text->centeredX = true;
		text->centeredY = true;

		SetFont(std::move(font));
		SetString(string);
		SetMaterial(std::move(material));
	}

	Text& Label::GetText()
	{
		return *text;
	}

	const Text& Label::GetText() const
	{
		return *text;
	}

	void Label::SetString(std::string_view string)
	{
		text->string = string;

		textWidth = text->GetLineWidth(1);
	}

	const std::string& Label::GetString() const
	{
		return text->string;
	}

	void Label::SetFont(Font::Ptr font)
	{
		text->font = std::move(font);
	}

	Font::Ptr Label::GetFont() const
	{
		return text->font;
	}

	void Label::SetMaterial(Material::Ptr material)
	{
		text->SetMaterial(std::move(material));
	}

	Material& Label::GetMaterial() const
	{
		return text->GetMaterial();
	}

	void Label::UpdateContent()
	{
		const float widgetWidth = GetAbsoluteBounds().width;

		if (textWidth > widgetWidth)
		{
			const float scalar = (widgetWidth / textWidth) * textScale;

			owner.scale = vec3(scalar);
		}
		else
		{
			owner.scale = vec3(textScale);
		}
	}
}

REFLECT_COMPONENT(gem::Label, gem::Widget)
	MEMBERS {
		REF_MEMBER(textScale)
	}
REF_END;
