// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Resource.h"
#include "Jewel3D/Reflection/Reflection.h"
#include "Jewel3D/Rendering/Rendering.h"

namespace Jwl
{
	struct CharData
	{
		int x;
		int y;
	};

	class Font : public Resource<Font>
	{
		REFLECT_PRIVATE;
	public:
		Font();
		~Font();

		bool Load(const std::string& filePath);
		bool Load(const std::string& filePath, TextureFilterMode filter);
		void Unload();

		//- Returns the real world unit width of the string.
		//- If the string is multi-line, the length of the longest line is returned.
		int GetStringWidth(const std::string& text) const;
		int GetStringHeight() const;

		const unsigned* GetTextures() const;
		const CharData* GetDimensions() const;
		const CharData* GetPositions() const;
		const CharData* GetAdvances() const;
		const bool* GetMasks() const;
		unsigned GetFontWidth() const;
		unsigned GetFontHeight() const;

		static unsigned GetVAO();
		static unsigned GetVBO();

	private:
		unsigned textures[94];
		//- Each character's dimensions.
		CharData dimensions[94];
		//- The position of each character.
		CharData positions[94];
		//- The distance from one character to the next.
		CharData advances[94];
		//- Whether the character is included in the font.
		bool masks[94];
		unsigned width  = 0;
		unsigned height = 0;

		static unsigned VBO;
		static unsigned VAO;
	};
}

REFLECT(Jwl::CharData)<>,
	MEMBERS<
		REF_MEMBER(x)<>,
		REF_MEMBER(y)<>
	>
REF_END;

REFLECT_SHAREABLE(Jwl::Font)
REFLECT(Jwl::Font) < Resource >,
	MEMBERS<
		REF_MEMBER(width)< ReadOnly >,
		REF_MEMBER(height)< ReadOnly >
	>
REF_END;
