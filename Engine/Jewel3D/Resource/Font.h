// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Application/Types.h"
#include "Resource.h"

namespace Jwl
{
	struct CharData
	{
		s32 x;
		s32 y;
	};

	class Font : public Resource<Font>
	{
	public:
		Font();
		~Font();

		//- Loads pre-packed *.font resources.
		bool Load(std::string filePath);
		void Unload();

		//- Returns the real world unit width of the string.
		//- If the string is multi-line, the length of the longest line is returned.
		s32 GetStringWidth(const std::string& text) const;
		s32 GetStringHeight() const;

		const u32* GetTextures() const;
		const CharData* GetDimensions() const;
		const CharData* GetPositions() const;
		const CharData* GetAdvances() const;
		const bool* GetMasks() const;
		u32 GetFontWidth() const;
		u32 GetFontHeight() const;

		static u32 GetVAO();
		static u32 GetVBO();

	private:
		u32 textures[94];
		//- Each character's dimensions.
		CharData dimensions[94];
		//- The position of each character.
		CharData positions[94];
		//- The distance from one character to the next.
		CharData advances[94];
		//- Whether the character is included in the font.
		bool masks[94];
		u32 width  = 0;
		u32 height = 0;

		static u32 VBO;
		static u32 VAO;
	};
}
