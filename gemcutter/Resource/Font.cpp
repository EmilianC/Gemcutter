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
	unsigned Font::VAO = 0;
	unsigned Font::VBO = 0;

	Font::~Font()
	{
		Unload();
	}

	bool Font::Load(std::string_view filePath)
	{
		if (VAO == GL_NONE)
		{
			// Prepare VBO's and VAO.
			unsigned verticesSize = sizeof(float) * 6 * 3;
			unsigned texCoordsSize = sizeof(float) * 6 * 2;
			unsigned normalsSize = sizeof(float) * 6 * 3;
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

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (std::byte*)nullptr + 0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (std::byte*)nullptr + verticesSize);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (std::byte*)nullptr + verticesSize + texCoordsSize);

			glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
			glBindVertexArray(GL_NONE);
		}

		/* Load Font from file */
		FILE* fontFile = fopen(filePath.data(), "rb");
		if (fontFile == nullptr)
		{
			Error("Font: ( %s )\nUnable to open file.", filePath.data());
			return false;
		}

		// Load bitmap data.
		TextureFilter filter;
		unsigned long int bitmapSize = 0;
		std::byte* bitmap = nullptr;
		defer{ free(bitmap); };

		// Read header.
		fread(&bitmapSize, sizeof(unsigned long int), 1, fontFile);
		fread(&width, sizeof(unsigned), 1, fontFile);
		fread(&height, sizeof(unsigned), 1, fontFile);
		fread(&filter, sizeof(TextureFilter), 1, fontFile);

		// Load Data.
		bitmap = static_cast<std::byte*>(malloc(sizeof(std::byte) * bitmapSize));
		fread(bitmap, sizeof(std::byte), bitmapSize, fontFile);
		fread(dimensions, sizeof(CharData), 94, fontFile);
		fread(positions, sizeof(CharData), 94, fontFile);
		fread(advances, sizeof(CharData), 94, fontFile);
		fread(masks, sizeof(bool), 94, fontFile);

		fclose(fontFile);

		// Upload data to OpenGL.
		glGenTextures(94, textures);

		std::byte* bitmapItr = bitmap;
		for (unsigned i = 0; i < 94; ++i)
		{
			if (!masks[i])
				continue;

			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ResolveFilterMag(filter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ResolveFilterMin(filter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			// Send the texture data.
			int numLevels = 1;
			if (ResolveMipMapping(filter))
			{
				float max = static_cast<float>(Max(dimensions[i].x, dimensions[i].y));
				numLevels = static_cast<int>(std::floor(std::log2(max))) + 1;
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

		return true;
	}

	void Font::Unload()
	{
		glDeleteTextures(94, textures);

		memset(textures, GL_NONE, sizeof(unsigned) * 94);
	}

	int Font::GetStringWidth(std::string_view text) const
	{
		int length = 0;
		int largest = INT_MIN;
		const int spaceWidth = GetSpaceWidth();

		for (unsigned i = 0; i < text.size(); ++i)
		{
			const char ch = text[i];
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
			else
			{
				if (i == text.size() - 1)
				{
					// The last character doesn't have an advance, only it's own total width.
					length += positions[ch - '!'].x + dimensions[ch - '!'].x;
				}
				else
				{
					length += advances[ch - '!'].x;
				}
			}
		}

		return Max(largest, length);
	}

	int Font::GetStringHeight() const
	{
		return dimensions['Z' - '!'].y;
	}

	int Font::GetSpaceWidth() const
	{
		return dimensions['Z' - '!'].x;
	}

	const unsigned* Font::GetTextures() const
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

	unsigned Font::GetFontWidth() const
	{
		return width;
	}

	unsigned Font::GetFontHeight() const
	{
		return height;
	}

	unsigned Font::GetVAO()
	{
		return VAO;
	}

	unsigned Font::GetVBO()
	{
		return VBO;
	}
}

REFLECT_RESOURCE(gem::Font) REF_END;
