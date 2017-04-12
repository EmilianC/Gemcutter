#include "FontEncoder.h"

#include <ft2build.h>
#include <Freetype/freetype.h>
#include <iostream>
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

bool FontEncoder::Validate(const Jwl::ConfigTable& data, unsigned loadedVersion) const
{
	switch (loadedVersion)
	{
	case 1:
		if (data.GetSize() != 3)
		{
			std::cout << "[e]Incorrect number of value entries." << std::endl;
			return false;
		}

		// Check Width
		if (!data.HasSetting("width"))
		{
			std::cout << "[e]Missing \"width\" value." << std::endl;
			return false;
		}
		
		if (data.GetInt("width") == 0)
		{
			std::cout << "[e]Width must be greater than 0." << std::endl;
			return false;
		}

		// Check Height
		if (!data.HasSetting("height"))
		{
			std::cout << "[e]Missing \"height\" value." << std::endl;
			return false;
		}

		if (data.GetInt("height") == 0)
		{
			std::cout << "[e]Height must be greater than 0." << std::endl;
			return false;
		}
	}

	return true;
}

bool FontEncoder::Convert(const std::string& source, const std::string& destination, const Jwl::ConfigTable& data) const
{
	std::string outputFile = destination;
	unsigned width = static_cast<unsigned>(data.GetInt("width"));
	unsigned height = static_cast<unsigned>(data.GetInt("height"));

	//- Append our new extension.
	if (outputFile.find(".font") == std::string::npos)
	{
		outputFile += ".font";
	}

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

	// Initialize FreeType2.
	if (FT_Init_FreeType(&library))
	{
		std::cout << "[e]FreeType failed to initialize." << std::endl;
		return false;
	}

	// Create the face data.
	if (FT_New_Face(library, source.c_str(), 0, &face))
	{
		std::cout << "[e]Input file could not be opened or processed." << std::endl;
		return false;
	}

	// Set font size.
	if (FT_Set_Char_Size(face, width * 64, height * 64, 96, 96))
	{
		std::cout << "[e]The width and height could not be processed." << std::endl;
		return false;
	}

	std::cout << "|Char|\t|Size|\t|Pos|\t|Advance|" << std::endl;

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
			std::cout << "[e]Glyph (" << c << ") could not be processed." << std::endl;
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

			std::cout << "[r]";
			masks[index] = true;
		}
		else
		{
			std::cout << "[e]";
			masks[index] = false;
		}
		std::cout << '\'' << c << "\'\t";

		dimensions[index].x = face->glyph->bitmap.width;
		dimensions[index].y = face->glyph->bitmap.rows;
		std::cout << dimensions[index].x << ' ' << dimensions[index].y << "\t";

		positions[index].x = face->glyph->bitmap_left;
		positions[index].y = face->glyph->bitmap_top - face->glyph->bitmap.rows;
		std::cout << positions[index].x << ' ' << positions[index].y << "\t";

		advances[index].x = face->glyph->advance.x / 64;
		advances[index].y = face->glyph->advance.y / 64;
		std::cout << advances[index].x << ' ' << advances[index].y << '\n';
	}

	// We don't need FreeType anymore.
	FT_Done_FreeType(library);

	// Save file.
	fontFile = fopen(outputFile.c_str(), "wb");
	if (fontFile == nullptr)
	{
		std::cout << "[e]Output file could not be created." << std::endl;
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
		std::cout << "[e]Failed to generate Font Binary\nOutput file could not be saved." << std::endl;
		return false;
	}
	else if (count == 0)
	{
		std::cout << "[e]Failed to generate Font Binary\n0 out of 94 characters loaded." << std::endl;
		return false;
	}
	else
	{
		if (count != 94)
		{
			std::cout << "[w]";
		}
		std::cout << count << " out of 94 characters loaded." << std::endl;

		std::cout << "[s]Font Binary created successfully" << std::endl;
		return true;
	}
}
