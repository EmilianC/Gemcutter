// Copyright (c) 2017 Emilian Cioca
#include "FontEncoder.h"
#include <gemcutter/Math/Math.h>
#include <gemcutter/Rendering/Rendering.h>
#include <gemcutter/Resource/Font.h>
#include <gemcutter/Utilities/String.h>

#include <array>
#include <freetype/config/ftheader.h>
#include <freetype/freetype.h>
#include <vector>

#define CURRENT_VERSION 3

constexpr float ESTIMATED_SPACE_EFFICIENCY = 0.8f;

unsigned ComputeRequiredDimensions(unsigned totalArea, unsigned maxCharacterHeight, gem::TextureFilter filter)
{
	float effectiveArea = totalArea * (1.0f / ESTIMATED_SPACE_EFFICIENCY);

	if (ResolveMipMapping(filter))
	{
		// We should prefer textures that are a power of 2.
		return gem::NextPowerOfTwo(static_cast<unsigned>(std::sqrtf(effectiveArea)));
	}

	unsigned result = static_cast<unsigned>(std::sqrtf(effectiveArea));

	// Make sure the result will be a multiple of the character height.
	result += result % maxCharacterHeight;

	return result;
}

FontEncoder::FontEncoder()
	: Encoder(CURRENT_VERSION)
{
}

gem::ConfigTable FontEncoder::GetDefault() const
{
	gem::ConfigTable defaultConfig;

	defaultConfig.SetInt("version", CURRENT_VERSION);
	defaultConfig.SetInt("width", 64);
	defaultConfig.SetInt("height", 64);
	defaultConfig.SetString("texture_filter", gem::EnumToString(gem::TextureFilter::Bilinear));

	return defaultConfig;
}

bool FontEncoder::Validate(const gem::ConfigTable& metadata, unsigned loadedVersion) const
{
	auto checkWidth = [](const gem::ConfigTable& data)
	{
		if (!data.HasSetting("width"))
		{
			gem::Error("Missing \"width\" value.");
			return false;
		}

		if (data.GetInt("width") == 0)
		{
			gem::Error("Width must be greater than 0.");
			return false;
		}

		return true;
	};

	auto checkHeight = [](const gem::ConfigTable& data)
	{
		if (!data.HasSetting("height"))
		{
			gem::Error("Missing \"height\" value.");
			return false;
		}

		if (data.GetInt("height") == 0)
		{
			gem::Error("Height must be greater than 0.");
			return false;
		}

		return true;
	};

	auto checkTextureFilter = [](const gem::ConfigTable& data, bool caseSensitive)
	{
		if (!data.HasSetting("texture_filter"))
		{
			gem::Error("Missing \"texture_filter\" value.");
			return false;
		}

		return gem::ValidateEnumValue<gem::TextureFilter>("texture_filter", data.GetString("texture_filter"), caseSensitive);
	};

	if (!checkWidth(metadata)) return false;
	if (!checkHeight(metadata)) return false;

	switch (loadedVersion)
	{
	case 1:
		if (metadata.GetSize() != 3)
		{
			gem::Error("Incorrect number of value entries.");
			return false;
		}
		break;

	case 2:
		if (!checkTextureFilter(metadata, false))
			return false;

		if (metadata.GetSize() != 4)
		{
			gem::Error("Incorrect number of value entries.");
			return false;
		}
		break;

	case 3:
		if (!checkTextureFilter(metadata, true))
			return false;

		if (metadata.GetSize() != 4)
		{
			gem::Error("Incorrect number of value entries.");
			return false;
		}
		break;

	default:
		gem::Error("Missing validation code for version %d", loadedVersion);
		return false;
	}

	return true;
}

bool FontEncoder::Convert(std::string_view source, std::string_view destination, const gem::ConfigTable& metadata) const
{
	const std::string outputFile = std::string(destination) + gem::ExtractFilename(source) + ".font";
	const unsigned width  = static_cast<unsigned>(metadata.GetInt("width"));
	const unsigned height = static_cast<unsigned>(metadata.GetInt("height"));
	const auto filter = gem::StringToEnum<gem::TextureFilter>(metadata.GetString("texture_filter")).value();

<<<<<<< HEAD
	// 10x10 grid of textures.
	const unsigned characterDimension = gem::Max(width, height);
	const unsigned textureWidth = characterDimension * 10;
	const unsigned textureHeight = textureWidth;
	const unsigned long int bitmapSize = textureWidth * textureHeight;
	auto* bitmapBuffer = static_cast<unsigned char*>(calloc(bitmapSize, sizeof(unsigned char)));
	defer { free(bitmapBuffer); };

	std::array<CharData, NUM_CHARACTERS> dimensions;
	std::array<CharData, NUM_CHARACTERS> positions;
	std::array<CharData, NUM_CHARACTERS> advances;
	std::array<bool, NUM_CHARACTERS> masks; // If a character is present in the font face.
	std::array<FT_Face> faces;              // This is probably incorrect.
=======
	gem::Font::Character characters[gem::Font::NUM_CHARACTERS] = {};
	unsigned char* bitmaps[gem::Font::NUM_CHARACTERS] = { nullptr };
	defer {
		for (auto* bitmap : bitmaps) free(bitmap);
	};
>>>>>>> 5b16803 ([WIP] Font encoding)

	FT_Library library;
	FT_Face face;

	if (FT_Init_FreeType(&library))
	{
		gem::Error("FreeType failed to initialize.");
		return false;
	}
	defer { FT_Done_FreeType(library); };

	// Create the face data.
	if (FT_New_Face(library, source.data(), 0, &face))
	{
		gem::Error("Input file could not be opened or processed.");
		return false;
	}

	// Set font size. 1pt = 1/72inch.
	if (FT_Set_Char_Size(face, width << 6, height << 6, 72, 72))
	{
		gem::Error("The width and height could not be processed.");
		return false;
	}

	// Retrieve width of spaces.
	int spaceWidth = 0;
	{
		FT_UInt charIndex = FT_Get_Char_Index(face, ' ');
		if (charIndex == 0 || FT_Load_Glyph(face, charIndex, FT_LOAD_RENDER))
		{
			gem::Error("Could not retrieve the width of spaces for the font.");
			return false;
		}

		spaceWidth = face->glyph->advance.x >> 6;
	}

	// Process ASCII characters from 33 ('!'), to 126 ('~').
	unsigned totalArea = 0;
	int stringHeight = 0;
	for (unsigned i = 0; i < gem::Font::NUM_CHARACTERS; ++i)
	{
		unsigned char c = static_cast<char>(i + 33);
		FT_UInt charIndex = FT_Get_Char_Index(face, c);
		if (charIndex == 0)
		{
			gem::Warning("Glyph (%c) is not available in the font.", c);
			continue;
		}

		if (FT_Load_Glyph(face, charIndex, FT_LOAD_RENDER))
		{
			gem::Warning("Glyph (%c) could not be rendered.", c);
			continue;
		}

		const unsigned glyphWidth  = face->glyph->bitmap.width;
		const unsigned glyphHeight = face->glyph->bitmap.rows;
		const unsigned glyphSize   = glyphWidth * glyphHeight;

		if (glyphWidth == 0 || glyphHeight == 0)
		{
			gem::Warning("Glyph (%c) is zero-sized.", c);
			continue;
		}

		if (glyphWidth > width)
		{
			gem::Warning("Glyph (%c) is too wide and is being discarded.", c);
			continue;
		}

		if (glyphHeight > height)
		{
			gem::Warning("Glyph (%c) is too tall and is being discarded.", c);
			continue;
		}


		totalArea += glyphSize;
		stringHeight = gem::Max(stringHeight, static_cast<int>(glyphHeight));

		bitmaps[i] = static_cast<unsigned char*>(malloc(glyphSize * sizeof(unsigned char)));
		memcpy(bitmaps[i], face->glyph->bitmap.buffer, glyphSize * sizeof(unsigned char));

		gem::FlipImage(bitmaps[i], glyphWidth, glyphHeight, 1);

		auto& character = characters[i];
		character.isValid = true;

		character.width = glyphWidth;
		character.height = face->glyph->bitmap.rows;

		character.offsetX = face->glyph->bitmap_left;
		character.offsetY = face->glyph->bitmap_top - face->glyph->bitmap.rows;

		character.advanceX = face->glyph->advance.x >> 6;
		character.advanceY = face->glyph->advance.y >> 6;
	}

	// Create packed texture
	auto PackTexture = [&](unsigned bitmapSize) -> unsigned char* {
		// Allocate Texture space.
		auto* bitmapBuffer = static_cast<unsigned char*>(calloc(bitmapSize, sizeof(unsigned char)));



		return bitmapBuffer;
	};

	unsigned char* bitmapBuffer = nullptr;
	unsigned textureWidth = ComputeRequiredDimensions(totalArea, static_cast<unsigned>(stringHeight), filter);
	while (true)
	{
		bitmapBuffer = PackTexture(textureWidth * textureWidth);
		if (bitmapBuffer)
			break;


	}
	defer { free(bitmapBuffer); };

	// Save file.
	FILE* fontFile = fopen(outputFile.c_str(), "wb");
	if (fontFile == nullptr)
	{
		gem::Error("Output file could not be created.");
		return false;
	}

	// Write header.
	fwrite(&textureWidth,  sizeof(textureWidth),  1, fontFile);
	fwrite(&textureHeight, sizeof(textureHeight), 1, fontFile);
	fwrite(&spaceWidth,    sizeof(spaceWidth),    1, fontFile);
	fwrite(&stringHeight,  sizeof(stringHeight),  1, fontFile);
	fwrite(&filter,        sizeof(filter),        1, fontFile);

	// Write Data.
	fwrite(bitmapBuffer.data(), sizeof(std::byte),  bitmapBuffer.size(), fontFile);
	fwrite(characters.data(),   sizeof(characters), 1,                   fontFile);

	int result = fclose(fontFile);

	// Report results.
	unsigned count = 0;
	std::string missingChars;
	for (unsigned i = 0; i < gem::Font::NUM_CHARACTERS; ++i)
	{
		if (characters[i].isValid)
		{
			count++;
		}
		else
		{
			missingChars += gem::FormatString("'%c' ", static_cast<char>(i + 33));
		}
	}

	if (result != 0)
	{
		gem::Error("Failed to generate Font Binary\nOutput file could not be saved.");
		return false;
	}
	else if (count == 0)
	{
		gem::Error("Failed to generate Font Binary\n0 characters loaded.");
		return false;
	}
	else
	{
		if (count != gem::Font::NUM_CHARACTERS)
		{
			gem::Warning("%d characters were not created.\n%s", gem::Font::NUM_CHARACTERS - count, missingChars.c_str());
		}

		return true;
	}
}

bool FontEncoder::Upgrade(gem::ConfigTable& metadata, unsigned loadedVersion) const
{
	switch (loadedVersion)
	{
	case 1:
		// Added texture_filter field.
		metadata.SetString("texture_filter", gem::EnumToString(gem::TextureFilter::Bilinear));
		break;

	case 2:
		// Enums became case sensitive.
		metadata.SetString("texture_filter", gem::FixEnumCasing(metadata.GetString("texture_filter"), gem::TextureFilter::Bilinear));
		break;
	}

	return true;
}
