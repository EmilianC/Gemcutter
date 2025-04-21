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
		TextureFilter filter = TextureFilter::Point;
		size_t textureWidth = 0;
		size_t textureHeight = 0;
		std::byte* bitmap = nullptr;
		defer{ free(bitmap); };

		// Read header.
		fread(&textureWidth,  sizeof(textureWidth),  1, fontFile);
		fread(&textureHeight, sizeof(textureHeight), 1, fontFile);
		fread(&width,         sizeof(width),         1, fontFile);
		fread(&height,        sizeof(height),        1, fontFile);
		fread(&filter,        sizeof(filter),        1, fontFile);

		// Load Data.
		bitmap = static_cast<std::byte*>(malloc(sizeof(std::byte) * textureWidth * textureHeight));
		fread(bitmap, sizeof(std::byte), textureWidth * textureHeight, fontFile);
		fread(dimensions.data(), sizeof(dimensions), 1, fontFile);
		fread(positions.data(),  sizeof(positions),  1, fontFile);
		fread(advances.data(),   sizeof(advances),   1, fontFile);
		fread(masks.data(),      sizeof(masks),      1, fontFile);

		fclose(fontFile);

		// Upload data to OpenGL.
		texture = Texture::MakeNew();
		texture->Create(textureWidth, textureHeight, TextureFormat::R_8, filter, TextureWrap::Clamp);

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

	Texture::Ptr Font::GetTexture() const
	{
		return texture;
	}

	std::span<const CharData> Font::GetDimensions() const
	{
		return dimensions;
	}

	std::span<const CharData> Font::GetPositions() const
	{
		return positions;
	}

	std::span<const CharData> Font::GetAdvances() const
	{
		return advances;
	}

	std::span<const bool> Font::GetMasks() const
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

REFLECT(gem::Font) BASES { REF_BASE(gem::ResourceBase) }
	MEMBERS {
		REF_PRIVATE_MEMBER(texture)
	}
REF_END;
