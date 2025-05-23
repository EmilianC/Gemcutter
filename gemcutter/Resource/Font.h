// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Resource/Resource.h"
#include "gemcutter/Resource/Shareable.h"

#include <array>
#include <span>
#include <string_view>

namespace gem
{
	struct CharData
	{
		int x;
		int y;
	};

	// A typeface that can be used to render text.
	// To be rendered, this must be set on an Entity's Text component.
	class Font : public Resource<Font>, public Shareable<Font>
	{
	public:
		static constexpr std::string_view Extension = ".font";

		~Font();

		// Loads pre-packed *.font resources.
		bool Load(std::string_view filePath);
		void Unload();

		// Returns the real world unit width of the string.
		// If the string is multi-line, the length of the longest line is returned.
		int GetStringWidth(std::string_view text) const;
		int GetStringHeight() const;
		// Returns the width required to advance forward by a space.
		int GetSpaceWidth() const;

		std::span<const unsigned> GetTextures() const;
		std::span<const CharData> GetDimensions() const;
		std::span<const CharData> GetPositions() const;
		std::span<const CharData> GetAdvances() const;
		std::span<const bool> GetMasks() const;
		unsigned GetFontWidth() const;
		unsigned GetFontHeight() const;

		static unsigned GetVAO();
		static unsigned GetVBO();

	private:
		std::array<unsigned, 94> textures;
		// Each character's dimensions.
		std::array<CharData, 94> dimensions;
		// The position of each character.
		std::array<CharData, 94> positions;
		// The distance from one character to the next.
		std::array<CharData, 94> advances;
		// Whether the character is included in the font.
		std::array<bool, 94> masks;

		unsigned width  = 0;
		unsigned height = 0;

		static unsigned VBO;
		static unsigned VAO;
	};
}
