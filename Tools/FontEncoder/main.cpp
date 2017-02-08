#include "Jewel3D/Application/CmdArgs.h"
#include "Jewel3D/Utilities/ScopeGuard.h"

#include <ft2build.h>
#include <Freetype/freetype.h>
#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

struct CharData
{
	int x = 0;
	int y = 0;
};

enum class ConsoleColor
{
	DarkBlue = 1,
	DarkGreen,
	DarkTeal,
	DarkRed,
	DarkPink,
	DarkYellow,
	Gray,
	DarkGray,
	Blue,
	Green,
	Teal,
	Red,
	Pink,
	Yellow,
	White
};

static void SetColor(ConsoleColor color)
{
	static HANDLE StdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(StdOutputHandle, (WORD)color);
}

void OutputUsage()
{
	std::cout << "Generates Jewel3D Font Binaries.\n"
		<< "Usage:\n"
		<< "FontEncoder.exe [input .tff] [commands] [output .font]\n"
		<< "Commands:\n"
		<< "width\t\t-w\tThe standard width resolution of a character.\n"
		<< "height\t\t-d\tThe standard height resolution of a character.\n";
}

int main()
{
	if (Jwl::GetArgc() != 7)
	{
		OutputUsage();
		return EXIT_FAILURE;
	}

	std::string inputFile;
	std::string outputFile;
	unsigned width;
	unsigned height;

	if (!Jwl::GetCommandLineArgument(1, inputFile) ||
		!Jwl::GetCommandLineArgument("-w", width) ||
		!Jwl::GetCommandLineArgument("-h", height) ||
		!Jwl::GetCommandLineArgument(6, outputFile))
	{
		OutputUsage();
		return EXIT_FAILURE;
	}

	if (width == 0)
	{
		std::cout << "Width must be greater than 0.\n";
		return EXIT_FAILURE;
	}

	if (height == 0)
	{
		std::cout << "Height must be greater than 0.\n";
		return EXIT_FAILURE;
	}

	if (inputFile.find(".ttf") == std::string::npos)
	{
		inputFile += ".ttf";
	}

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
		SetColor(ConsoleColor::Red);
		std::cout << "FreeType failed to initialize.\n";
		SetColor(ConsoleColor::Gray);
		return EXIT_FAILURE;
	}

	// Create the face data.
	if (FT_New_Face(library, inputFile.c_str(), 0, &face))
	{
		SetColor(ConsoleColor::Red);
		std::cout << "Input file could not be opened or processed.\n";
		SetColor(ConsoleColor::Gray);
		return EXIT_FAILURE;
	}

	// Set font size.
	if (FT_Set_Char_Size(face, width * 64, height * 64, 96, 96))
	{
		SetColor(ConsoleColor::Red);
		std::cout << "The width and height could not be processed.\n";
		SetColor(ConsoleColor::Gray);
		return EXIT_FAILURE;
	}
	
	std::cout << "\n|Char|\t\t|Size|\t\t|Pos|\t\t|Advance|\n";

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
			SetColor(ConsoleColor::Red);
			std::cout << "Glyph (" << c << ") could not be processed.\n";
			SetColor(ConsoleColor::Gray);
			return EXIT_FAILURE;
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

			SetColor(ConsoleColor::Green);
			masks[index] = true;
		}
		else
		{
			SetColor(ConsoleColor::Red);
			masks[index] = false;
		}
		std::cout << '\'' << c << "\'\t\t";

		dimensions[index].x = face->glyph->bitmap.width;
		dimensions[index].y = face->glyph->bitmap.rows;
		std::cout << dimensions[index].x << ' ' << dimensions[index].y << "\t\t";

		positions[index].x = face->glyph->bitmap_left;
		positions[index].y = face->glyph->bitmap_top - face->glyph->bitmap.rows;
		std::cout << positions[index].x << ' ' << positions[index].y << "\t\t";

		advances[index].x = face->glyph->advance.x / 64;
		advances[index].y = face->glyph->advance.y / 64;
		std::cout << advances[index].x << ' ' << advances[index].y << '\n';
	}
	SetColor(ConsoleColor::Gray);
	std::cout << '\n';

	// We don't need FreeType anymore.
	FT_Done_FreeType(library);

	// Save file.
	fontFile = fopen(outputFile.c_str(), "wb");
	if (fontFile == nullptr)
	{
		SetColor(ConsoleColor::Red);
		std::cout << "Output file could not be created.\n";
		SetColor(ConsoleColor::Gray);
		return EXIT_FAILURE;
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
		if (val) count++;

	if (result != 0)
	{
		SetColor(ConsoleColor::Red);
		std::cout << "-Failed to generate Jewel3D Font Binary-\nOutput file could not be saved.\n";

		SetColor(ConsoleColor::Gray);
		return EXIT_FAILURE;
	}
	else if (count == 0)
	{
		SetColor(ConsoleColor::Red);
		std::cout << "-Failed to generate Jewel3D Font Binary-\n0 out of 94 characters loaded.\n";

		SetColor(ConsoleColor::Gray);
		return EXIT_FAILURE;
	}
	else
	{
		SetColor(ConsoleColor::Green);
		std::cout << "-Jewel3D Font Binary created successfully-\n";

		if (count != 94)
		{
			SetColor(ConsoleColor::Yellow);
		}

		std::cout << count << " out of 94 characters loaded\n";

		SetColor(ConsoleColor::Gray);
		return EXIT_SUCCESS;
	}
}
