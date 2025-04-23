// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Rendering/Renderable.h"
#include "gemcutter/Resource/Font.h"
#include "gemcutter/Resource/VertexArray.h"

#include <string>

namespace gem
{
	namespace detail { struct StaleStringMesh : public Tag<StaleStringMesh> {}; }

	// Causes text to render at the entity's position.
	class Text : public Renderable
	{
	public:
		Text(Entity& owner);
		Text(Entity& owner, Font::Ptr font);
		Text(Entity& owner, std::string string);
		Text(Entity& owner, Material::Ptr material);
		Text(Entity& owner, Font::Ptr font, std::string string, Material::Ptr material);

		void SetString(std::string);
		const std::string& GetString() const;

		void SetFont(Font::Ptr);
		Font::Ptr GetFont() const;

		void SetCenteredX(bool);
		bool GetCenteredX() const;

		void SetCenteredY(bool);
		bool GetCenteredY() const;

		// Set extra spacing between letters.
		void SetKerning(float);
		float GetKerning() const;

		unsigned GetNumLines() const;
		float GetLineWidth(unsigned line) const;

		static void UpdateAll();

	private:
		void UpdateStringBuffer();
		void CreateStringBuffer(unsigned requiredSize);
		void ResizeStringBuffer(unsigned requiredSize);

		std::string string;
		Font::Ptr font;

		bool centeredX = false;
		bool centeredY = false;

		float kerning = 0.0f;

		VertexBuffer::Ptr stringBuffer;

	public:
		PRIVATE_MEMBER(Text, string);
		PRIVATE_MEMBER(Text, font);
		PRIVATE_MEMBER(Text, centeredX);
		PRIVATE_MEMBER(Text, centeredY);
		PRIVATE_MEMBER(Text, kerning);
	};
}
