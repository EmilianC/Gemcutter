// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Resource/Resource.h"
#include "gemcutter/Resource/Shareable.h"
#include "gemcutter/Resource/Texture.h"

#include <array>
#include <span>
#include <string_view>

namespace gem
{
	// A typeface that can be used to render text.
	// To be rendered, this must be set on an Entity's Text component.
	class Font : public Resource<Font>, public Shareable<Font>
	{
	public:
		static constexpr std::string_view Extension = ".font";

		// ASCII characters from 33 ('!'), to 126 ('~').
		static constexpr unsigned NUM_CHARACTERS = 94;
		struct Char
		{
			// Whether the character is available in the font.
			bool isValid;

			// The character's dimensions.
			int width;
			int height;
			// The local position of the character.
			int offsetX;
			int offsetY;
			// The distance to step to the next character.
			int advanceX;
			int advanceY;
			// The character's layout in the texture atlas.
			float UvLeft;
			float UvRight;
			float UvTop;
			float UvBottom;
		};

		~Font();

		// Loads pre-packed *.font resources.
		bool Load(std::string_view filePath);
		void Unload();

		// Returns the real world unit width of the string.
		// If the string is multi-line, the length of the longest line is returned.
		int GetStringWidth(std::string_view text) const;
		// Returns the width required to advance forward by a space.
		int GetSpaceWidth() const;
		// Returns the height of the tallest character available in the font.
		int GetStringHeight() const;

		Texture* GetTexture() const;

		static unsigned GetVAO();
		static unsigned GetVBO();

		std::span<const Char> GetCharacters() const;
	private:
		Texture::Ptr texture;

		int spaceWidth = 0;
		int stringHeight = 0;


		static unsigned VBO;
		static unsigned VAO;
		std::array<Char, NUM_CHARACTERS> characters;

	public:
		PRIVATE_MEMBER(Font, texture);
	};
}
