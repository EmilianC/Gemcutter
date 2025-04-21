// Copyright (c) 2017 Emilian Cioca
#include "FontEncoder.h"
#include <gemcutter/Math/Math.h>
#include <gemcutter/Rendering/Rendering.h>
#include <gemcutter/Utilities/String.h>

#include <array>
#include <freetype/config/ftheader.h>
#include <freetype/freetype.h>
#include <vector>

#define CURRENT_VERSION 3

constexpr unsigned NUM_CHARACTERS = 94;

struct CharData
{
	int x = 0;
	int y = 0;
};

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

	FT_Library library;

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

	// Process ASCII characters from 33 ('!'), to 126 ('~').
	for (unsigned i = 0; i < NUM_CHARACTERS; ++i)
	{
		unsigned char c = static_cast<char>(i + 33);
		auto charIndex = FT_Get_Char_Index(face, c);

		// Prepare bitmap.
		if (FT_Load_Glyph(face, charIndex, FT_LOAD_RENDER))
		{
			gem::Error("Glyph (%c) could not be processed.", c);
			return false;
		}

		dimensions[i].x = face->glyph->bitmap.width;
		dimensions[i].y = face->glyph->bitmap.rows;

		positions[i].x = face->glyph->bitmap_left;
		positions[i].y = face->glyph->bitmap_top - face->glyph->bitmap.rows;

		advances[i].x = face->glyph->advance.x >> 6;
		advances[i].y = face->glyph->advance.y >> 6;

		if (charIndex == 0 || (face->glyph->bitmap.width * face->glyph->bitmap.rows == 0))
		{
			continue;
		}

		if (face->glyph->bitmap.width > width)
		{
			Jwl::Error("Glyph (%c) is too wide", c);
			return false;
		}

		if (face->glyph->bitmap.rows > (int)height)
		{
			Jwl::Error("Glyph (%c) is too tall", c);
			return false;
		}

		masks[i] = true;
	}

	unsigned char* writer = bitmapBuffer;
	writer += (i / 10u) * textureWidth * characterDimension;
	writer += (i % 10u) * characterDimension;

	// Save the bitmap as a flipped image.
	for (int y = face->glyph->bitmap.rows - 1; y-- > 0;)
	{
		for (int x = 0; x < face->glyph->bitmap.width; ++x)
		{
			unsigned char pixel = face->glyph->bitmap.buffer[face->glyph->bitmap.width * y + x];

			*(writer + x) = pixel;
		}

		// Step to the next row.
		writer += textureWidth;
	}

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
	fwrite(&width,         sizeof(width),         1, fontFile);
	fwrite(&height,        sizeof(height),        1, fontFile);
	fwrite(&filter,        sizeof(filter),        1, fontFile);

	// Write Data.
	const size_t bitmapSize = bitmapBuffer.size();
	fwrite(bitmapBuffer.data(), sizeof(std::byte),  bitmapSize, fontFile);
	fwrite(dimensions.data(),   sizeof(dimensions), 1,          fontFile);
	fwrite(positions.data(),    sizeof(positions),  1,          fontFile);
	fwrite(advances.data(),     sizeof(advances),   1,          fontFile);
	fwrite(masks.data(),        sizeof(masks),      1,          fontFile);

	int result = fclose(fontFile);

	// Report results.
	unsigned count = 0;
	std::string missingChars;
	for (unsigned i = 0; i < NUM_CHARACTERS; ++i)
	{
		if (masks[i])
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
		if (count != NUM_CHARACTERS)
		{
			gem::Warning("%d characters were not created.\n%s", NUM_CHARACTERS - count, missingChars.c_str());
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
