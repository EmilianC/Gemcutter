#include "FontEncoder.h"

#include <ft2build.h>
#include <Freetype/freetype.h>
#include <vector>

#define CURRENT_VERSION 1

std::unique_ptr<Jwl::Encoder> GetEncoder()
{
	return std::make_unique<FontEncoder>();
}

struct CharData
{
	int x = 0;
	int y = 0;
};

FontEncoder::FontEncoder()
	: Encoder(CURRENT_VERSION)
{
}

Jwl::ConfigTable FontEncoder::GetDefault() const
{
	Jwl::ConfigTable defaultConfig;

	defaultConfig.SetValue("version", CURRENT_VERSION);
	defaultConfig.SetValue("width", 64);
	defaultConfig.SetValue("height", 64);

	return defaultConfig;
}

bool FontEncoder::Validate(const Jwl::ConfigTable& metadata, unsigned loadedVersion) const
{
	switch (loadedVersion)
	{
	case 1:
		// Check Width
		if (!metadata.HasSetting("width"))
		{
			Jwl::Error("Missing \"width\" value.");
			return false;
		}
		
		if (metadata.GetInt("width") == 0)
		{
			Jwl::Error("Width must be greater than 0.");
			return false;
		}

		// Check Height
		if (!metadata.HasSetting("height"))
		{
			Jwl::Error("Missing \"height\" value.");
			return false;
		}

		if (metadata.GetInt("height") == 0)
		{
			Jwl::Error("Height must be greater than 0.");
			return false;
		}

		if (metadata.GetSize() != 3)
		{
			Jwl::Error("Incorrect number of value entries.");
			return false;
		}
	}

	return true;
}

bool FontEncoder::Convert(const std::string& source, const std::string& destination, const Jwl::ConfigTable& metadata) const
{
	const std::string outputFile = destination + Jwl::ExtractFilename(source) + ".font";
	const unsigned width = static_cast<unsigned>(metadata.GetInt("width"));
	const unsigned height = static_cast<unsigned>(metadata.GetInt("height"));

	// File preparation.
	FILE* fontFile = nullptr;
	std::vector<unsigned char> bitmapBuffer;
	CharData dimensions[94] = { CharData() };
	CharData positions[94] = { CharData() };
	CharData advances[94] = { CharData() };
	bool masks[94] = { false }; // If a character is present in the font face.

	// FreeType variables.
	FT_Library library;
	FT_Face face;

	if (FT_Init_FreeType(&library))
	{
		Jwl::Error("FreeType failed to initialize.");
		return false;
	}
	defer { FT_Done_FreeType(library); };

	// Create the face data.
	if (FT_New_Face(library, source.c_str(), 0, &face))
	{
		Jwl::Error("Input file could not be opened or processed.");
		return false;
	}

	// Set font size.
	if (FT_Set_Char_Size(face, width * 64, height * 64, 96, 96))
	{
		Jwl::Error("The width and height could not be processed.");
		return false;
	}

	// Estimation of the buffer size we will need.
	bitmapBuffer.reserve(width * height * 94);

	// Process ASCII characters from 33 ('!'), to 126 ('~').
	for (unsigned char c = 33; c < 127; c++)
	{
		unsigned index = c - 33;
		auto charIndex = FT_Get_Char_Index(face, c);

		// Prepare bitmap.
		if (FT_Load_Glyph(face, charIndex, FT_LOAD_RENDER))
		{
			Jwl::Error("Glyph (%c) could not be processed.", c);
			return false;
		}

		if (charIndex != 0 &&
			face->glyph->bitmap.width * face->glyph->bitmap.rows != 0)
		{
			// Save the bitmap as a flipped image.
			for (int i = face->glyph->bitmap.rows - 1; i >= 0; i--)
			{
				for (int j = 0; j < face->glyph->bitmap.width; j++)
				{
					bitmapBuffer.push_back(face->glyph->bitmap.buffer[face->glyph->bitmap.width * i + j]);
				}
			}

			masks[index] = true;
		}
		else
		{
			masks[index] = false;
		}

		dimensions[index].x = face->glyph->bitmap.width;
		dimensions[index].y = face->glyph->bitmap.rows;

		positions[index].x = face->glyph->bitmap_left;
		positions[index].y = face->glyph->bitmap_top - face->glyph->bitmap.rows;

		advances[index].x = face->glyph->advance.x / 64;
		advances[index].y = face->glyph->advance.y / 64;
	}

	// Save file.
	fontFile = fopen(outputFile.c_str(), "wb");
	if (fontFile == nullptr)
	{
		Jwl::Error("Output file could not be created.");
		return false;
	}

	// Write header.
	unsigned long int bitmapSize = bitmapBuffer.size();
	fwrite(&bitmapSize, sizeof(unsigned long int), 1, fontFile);
	fwrite(&width, sizeof(unsigned), 1, fontFile);
	fwrite(&height, sizeof(unsigned), 1, fontFile);

	// Write Data.
	fwrite(bitmapBuffer.data(), sizeof(unsigned char), bitmapSize, fontFile);
	fwrite(dimensions, sizeof(CharData), 94, fontFile);
	fwrite(positions, sizeof(CharData), 94, fontFile);
	fwrite(advances, sizeof(CharData), 94, fontFile);
	fwrite(masks, sizeof(bool), 94, fontFile);

	auto result = fclose(fontFile);

	// Report results.
	unsigned count = 0;
	for (bool val : masks)
	{
		if (val) count++;
	}

	if (result != 0)
	{
		Jwl::Error("Failed to generate Font Binary\nOutput file could not be saved.");
		return false;
	}
	else if (count == 0)
	{
		Jwl::Error("Failed to generate Font Binary\n0 out of 94 characters loaded.");
		return false;
	}
	else
	{
		if (count != 94)
		{
			Jwl::Warning("%d characters were not created.", 94 - count);
		}
		
		return true;
	}
}
