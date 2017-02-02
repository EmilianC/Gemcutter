#include "Jewel3D/Application/CmdArgs.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <Windows.h>
#include <ft2build.h>
#include <Freetype/freetype.h>

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
	std::cout << "Generate Jewel3D Font Binaries.\n"
		<< "Usage:\n"
		<< "FontEncoder.exe [input .tff] [commands] [output file]\n"
		<< "Commands:\n"
		<< "width\t\t-w\tThe standard width resolution of a character.\n"
		<< "height\t\t-d\tThe standard height resolution of a character.\n";
}

int main(int argc, char* argv[])
{
	if (argc != 7)
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

	// File preparation
	FILE *fontFile = nullptr;
	std::vector<unsigned char> bitmapBuffer;
	std::vector<int> dimensions;
	std::vector<int> positions;
	std::vector<int> advance;
	std::vector<unsigned char> mask; // If a character is present in the font face (true/false)
	
	// FreeType variables
	FT_Library library;
	FT_Face face;

	// Initialize FreeType 2
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

	// Process ASCII characters from 33 ('!'), to 126 ('~').
	for (unsigned char c = 33; c < 127; c++)
	{
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
			// Add bitmap and data to file buffer.
			for (int i = 0; i < face->glyph->bitmap.width * face->glyph->bitmap.rows; i++)
			{
				//TODO: flip the texture
				unsigned char pixel = face->glyph->bitmap.buffer[i];
				bitmapBuffer.push_back(pixel); // r
				bitmapBuffer.push_back(pixel); // g
				bitmapBuffer.push_back(pixel); // b
				bitmapBuffer.push_back(pixel); // a
			}

			SetColor(ConsoleColor::Green);
			mask.push_back((unsigned char)1);
		}
		else
		{
			SetColor(ConsoleColor::DarkRed);
			mask.push_back((unsigned char)0);
		}
		std::cout << '\'' << c << "\'\t\t";

		dimensions.push_back(face->glyph->bitmap.width);
		std::cout << dimensions.back() << ' ';
		dimensions.push_back(face->glyph->bitmap.rows);
		std::cout << dimensions.back() << "\t\t";

		positions.push_back(face->glyph->bitmap_left);
		std::cout << positions.back() << ' ';
		positions.push_back(face->glyph->bitmap_top - face->glyph->bitmap.rows);
		std::cout << positions.back() << "\t\t";

		advance.push_back(face->glyph->advance.x / 64);
		std::cout << advance.back() << ' ';
		advance.push_back(face->glyph->advance.y / 64);
		std::cout << advance.back() << '\n';
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

	// bitmapBuffer
	fwrite(&bitmapBuffer[0], sizeof(unsigned char), bitmapSize, fontFile);
	// dimensions
	fwrite(&dimensions[0], sizeof(int), dimensions.size(), fontFile);
	// positions
	fwrite(&positions[0], sizeof(int), positions.size(), fontFile);
	// advances
	fwrite(&advance[0], sizeof(int), advance.size(), fontFile);
	// mask
	fwrite(&mask[0], sizeof(unsigned char), mask.size(), fontFile);

	fclose(fontFile);

	// Report results.
	int count = 0;
	for (unsigned char val : mask)
	{
		if (val != 0)
		{
			count++;
		}
	}

	if (count == 0)
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
