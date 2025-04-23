// Copyright (c) 2017 Emilian Cioca
#include "Font.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Math.h"
#include "gemcutter/Rendering/Rendering.h"
#include "gemcutter/Resource/Texture.h"
#include "gemcutter/Utilities/ScopeGuard.h"

#include <cstdio>
#include <GL/glew.h>

namespace gem
{
	Font::~Font()
	{
		Unload();
	}

	bool Font::Load(std::string_view filePath)
	{
		/* Load Font from file */
		FILE* fontFile = fopen(filePath.data(), "rb");
		if (fontFile == nullptr)
		{
			Error("Font: ( %s )\nUnable to open file.", filePath.data());
			return false;
		}

		// Load bitmap data.
		TextureFilter filter = TextureFilter::Point;
		size_t textureWidth = 0;
		size_t textureHeight = 0;
		std::byte* bitmap = nullptr;
		defer{ free(bitmap); };

		// Read header.
		fread(&textureWidth,  sizeof(textureWidth),  1, fontFile);
		fread(&textureHeight, sizeof(textureHeight), 1, fontFile);
		fread(&spaceWidth,    sizeof(spaceWidth),    1, fontFile);
		fread(&stringHeight,  sizeof(stringHeight),  1, fontFile);
		fread(&filter,        sizeof(filter),        1, fontFile);
		const size_t textureSize = textureWidth * textureHeight;

		// Load Data.
		bitmap = static_cast<std::byte*>(malloc(sizeof(std::byte) * textureSize));
		fread(bitmap, sizeof(std::byte), textureSize, fontFile);
		fread(characters.data(), sizeof(characters), 1, fontFile);

		fclose(fontFile);

		// Upload data to OpenGL.
		texture = Texture::MakeNew();
		texture->Create(static_cast<unsigned>(textureWidth), static_cast<unsigned>(textureHeight), TextureFormat::R_8, filter, TextureWrap::Clamp);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		texture->SetData(bitmap, TextureFormat::R_8);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		return true;
	}

	void Font::Unload()
	{
		texture->Unload();
	}

	int Font::GetStringWidth(std::string_view text) const
	{
		int length = 0;
		int largest = INT_MIN;

		for (unsigned i = 0; i < text.size(); ++i)
		{
			const char ch = text[i];
			const Char& data = characters[ch - 33];

			if (ch == ' ')
			{
				length += spaceWidth;
			}
			else if (ch == '\n')
			{
				largest = Max(largest, length);
				length = 0;
			}
			else if (ch == '\t')
			{
				length += spaceWidth * 4;
			}
			else if (data.isValid)
			{
				if (i == text.size() - 1)
				{
					// The last character doesn't have an advance, only it's own total width.
					length += data.offsetX + data.width;
				}
				else
				{
					length += data.advanceX;
				}
			}
		}

		return Max(largest, length);
	}

	int Font::GetStringHeight() const
	{
		return stringHeight;
	}

	int Font::GetSpaceWidth() const
	{
		return spaceWidth;
	}

	Texture* Font::GetTexture() const
	{
		return texture.get();
	}

	std::span<const Font::Char> Font::GetCharacters() const
	{
		return characters;
	}
}

REFLECT(gem::Font) BASES { REF_BASE(gem::ResourceBase) }
	MEMBERS {
		REF_PRIVATE_MEMBER(texture)
	}
REF_END;
