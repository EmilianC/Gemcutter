// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Font.h"
#include "Texture.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Math/Math.h"
#include "Jewel3D/Rendering/Rendering.h"
#include "Jewel3D/Utilities/ScopeGuard.h"
#include "Jewel3D/Utilities/String.h"

#include <algorithm>
#include <cstdio>
#include <GLEW/GL/glew.h>

namespace Jwl
{
	u32 Font::VAO = 0;
	u32 Font::VBO = 0;

	Font::Font()
	{
		memset(textures,	0, sizeof(u32) * 94);
		memset(dimensions,	0, sizeof(CharData) * 94);
		memset(positions,	0, sizeof(CharData) * 94);
		memset(advances,	0, sizeof(CharData) * 94);
		memset(masks,		false, sizeof(bool) * 94);
	}

	Font::~Font()
	{
		Unload();
	}

	bool Font::Load(std::string filePath)
	{
		if (VAO == GL_NONE)
		{
			// Prepare VBO's and VAO.
			u32 verticesSize = sizeof(f32) * 6 * 3;
			u32 texCoordsSize = sizeof(f32) * 6 * 2;
			u32 normalsSize = sizeof(f32) * 6 * 3;
			GLfloat data[48] =
			{
				/* Vertices */
				// face 1
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				// face 2
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,

				/* Texcoords */
				// face 1
				0.0f, 0.0f,
				1.0f, 0.0f,
				0.0f, 1.0f,
				// face 2
				1.0f, 0.0f,
				1.0f, 1.0f,
				0.0f, 1.0f,

				/* Normals */
				// face 1
				0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f,
				// face 2
				0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f
			};

			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);

			glEnableVertexAttribArray(0); // Position
			glEnableVertexAttribArray(1); // UV
			glEnableVertexAttribArray(2); // Normals

			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, verticesSize + texCoordsSize + normalsSize, data, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(verticesSize));
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(verticesSize + texCoordsSize));

			glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
			glBindVertexArray(GL_NONE);
		}

		/* Load Font from file */
		auto ext = ExtractFileExtension(filePath);
		if (ext.empty())
		{
			filePath += ".font";
		}
		else if (!CompareLowercase(ExtractFileExtension(filePath), ".font"))
		{
			Error("Font: ( %s )\nAttempted to load unknown file type as a font.", filePath.c_str());
			return false;
		}

		FILE* fontFile = fopen(filePath.c_str(), "rb");
		if (fontFile == nullptr)
		{
			Error("Font: ( %s )\nUnable to open file.", filePath.c_str());
			return false;
		}

		// Load bitmap data.
		TextureFilterMode filter;
		u32 bitmapSize = 0;
		u8* bitmap = nullptr;
		defer{ free(bitmap); };

		// Read header.
		fread(&bitmapSize, sizeof(u32), 1, fontFile);
		fread(&width, sizeof(u32), 1, fontFile);
		fread(&height, sizeof(u32), 1, fontFile);
		fread(&filter, sizeof(TextureFilterMode), 1, fontFile);

		// Load Data.
		bitmap = static_cast<u8*>(malloc(sizeof(u8) * bitmapSize));
		fread(bitmap, sizeof(u8), bitmapSize, fontFile);
		fread(dimensions, sizeof(CharData), 94, fontFile);
		fread(positions, sizeof(CharData), 94, fontFile);
		fread(advances, sizeof(CharData), 94, fontFile);
		fread(masks, sizeof(bool), 94, fontFile);

		fclose(fontFile);

		// Upload data to OpenGL.
		glGenTextures(94, textures);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		u8* bitmapItr = bitmap;
		for (u32 i = 0; i < 94; i++)
		{
			if (!masks[i])
				continue;

			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ResolveFilterMagMode(filter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ResolveFilterMinMode(filter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			// Send the texture data.
			s32 numLevels = 1;
			if (ResolveMipMapping(filter))
			{
				f32 max = static_cast<f32>(Max(dimensions[i].x, dimensions[i].y));
				numLevels = static_cast<s32>(std::floor(std::log2(max))) + 1;
			}

			glTexStorage2D(GL_TEXTURE_2D, numLevels, GL_R8, dimensions[i].x, dimensions[i].y);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dimensions[i].x, dimensions[i].y, GL_RED, GL_UNSIGNED_BYTE, bitmapItr);

			if (numLevels > 1)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}

			// Move the pointer to the next set of data.
			bitmapItr += dimensions[i].x * dimensions[i].y;
		}

		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		// Restore default unpack behaviour.
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		return true;
	}

	void Font::Unload()
	{
		glDeleteTextures(94, textures);

		memset(textures, GL_NONE, sizeof(u32) * 94);
	}

	s32 Font::GetStringWidth(const std::string& text) const
	{
		s32 length = 0;
		s32 largest = INT_MIN;

		for (char ch : text)
		{
			if (ch == ' ')
			{
				length += advances['Z' - '!'].x;
			}
			else if (ch == '\n')
			{
				largest = Max(largest, length);
				length = 0;
			}
			else if (ch == '\t')
			{
				length += advances['Z' - '!'].x * 4;
			}
			else
			{
				length += advances[ch - '!'].x;
			}
		}

		return Max(largest, length);
	}

	s32 Font::GetStringHeight() const
	{
		return dimensions['Z' - '!'].y;
	}

	const u32* Font::GetTextures() const
	{
		return textures;
	}

	const CharData* Font::GetDimensions() const
	{
		return dimensions;
	}

	const CharData* Font::GetPositions() const
	{
		return positions;
	}

	const CharData* Font::GetAdvances() const
	{
		return advances;
	}

	const bool* Font::GetMasks() const
	{
		return masks;
	}

	u32 Font::GetFontWidth() const
	{
		return width;
	}

	u32 Font::GetFontHeight() const
	{
		return height;
	}

	u32 Font::GetVAO()
	{
		return VAO;
	}

	u32 Font::GetVBO()
	{
		return VBO;
	}
}
