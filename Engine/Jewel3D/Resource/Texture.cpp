// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Texture.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Utilities/ScopeGuard.h"
#include "Jewel3D/Utilities/String.h"

#include <algorithm>
#include <GLEW/GL/glew.h>
#include <SOIL/SOIL.h>

namespace Jwl
{
	Texture::~Texture()
	{
		Unload();
	}

	void Texture::CreateTexture(unsigned _width, unsigned _height, TextureFormat _format, TextureFilterMode _filter, TextureWrapModes _wrapModes, float _anisotropicLevel, unsigned _numSamples)
	{
		ASSERT(hTex == 0, "Texture already has a texture loaded.");
		ASSERT(_anisotropicLevel >= 1.0f && _anisotropicLevel <= 16.0f, "'anisotropicLevel' must be in the range of [1, 16].");
		ASSERT(_numSamples == 1 || _numSamples == 2 || _numSamples == 4 || _numSamples == 8 || _numSamples == 16, "'numSamples' must be a power of 2 between 1 and 16.");

		width = static_cast<int>(_width);
		height = static_cast<int>(_height);
		format = _format;
		filter = _filter;
		wrapModes = _wrapModes;
		anisotropicLevel = _anisotropicLevel;
		numSamples = _numSamples;

		glGenTextures(1, &hTex);

		if (numSamples == 1)
		{
			glBindTexture(GL_TEXTURE_2D, hTex);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ResolveFilterMagMode(filter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ResolveFilterMinMode(filter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ResolveWrapMode(wrapModes.x));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ResolveWrapMode(wrapModes.y));
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropicLevel);

			unsigned numLevels = CountMipLevels(width, height, filter);
			glTexStorage2D(GL_TEXTURE_2D, numLevels, ResolveFormat(format), width, height);

			if (numLevels > 1)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}

			target = GL_TEXTURE_2D;
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hTex);
			glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numSamples, ResolveFormat(format), width, height, false);

			target = GL_TEXTURE_2D_MULTISAMPLE;
		}

		glBindTexture(target, GL_NONE);
	}

	bool Texture::Load(std::string filePath)
	{
		ASSERT(hTex == 0, "Texture already has a texture loaded.");

		auto ext = ExtractFileExtension(filePath);
		if (ext.empty() || CompareLowercase(ext, ".texture"))
		{
			if (ext.empty())
			{
				filePath += ".texture";
			}

			FILE* fontFile = fopen(filePath.c_str(), "rb");
			if (fontFile == nullptr)
			{
				Error("Texture: ( %s )\nUnable to open file.", filePath.c_str());
				return false;
			}
			defer { fclose(fontFile); };

			// Read header.
			bool isCubeMap = false;
			fread(&isCubeMap, sizeof(bool), 1, fontFile);
			fread(&width, sizeof(unsigned), 1, fontFile);
			fread(&height, sizeof(unsigned), 1, fontFile);
			fread(&format, sizeof(TextureFormat), 1, fontFile);
			fread(&filter, sizeof(TextureFilterMode), 1, fontFile);
			fread(&wrapModes.x, sizeof(TextureWrapMode), 1, fontFile);
			fread(&wrapModes.y, sizeof(TextureWrapMode), 1, fontFile);
			fread(&anisotropicLevel, sizeof(float), 1, fontFile);

			unsigned textureSize = width * height;
			if (format == TextureFormat::RGB_8)
			{
				textureSize *= 3;
			}
			else if (format == TextureFormat::RGBA_8)
			{
				textureSize *= 4;
			}
			else
			{
				Error("Texture: ( %s )\nUnsupported format. Must have 3 or 4 color channels.", filePath.c_str());
				return false;
			}

			if (isCubeMap)
			{
				unsigned char* image = static_cast<unsigned char*>(malloc(sizeof(unsigned char) * textureSize * 6));
				defer{ free(image); };

				fread(image, sizeof(unsigned char), textureSize * 6, fontFile);

				glGenTextures(1, &hTex);
				glBindTexture(GL_TEXTURE_CUBE_MAP, hTex);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, ResolveFilterMagMode(filter));
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, ResolveFilterMinMode(filter));
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropicLevel);

				unsigned numLevels = CountMipLevels(width, height, filter);
				if (CountChannels(format) == 3)
				{
					glTexStorage2D(GL_TEXTURE_CUBE_MAP, numLevels, GL_RGB8, width, height);

					for (unsigned i = 0; i < 6; i++)
					{
						glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image + (textureSize * i));
					}
				}
				else
				{
					glTexStorage2D(GL_TEXTURE_CUBE_MAP, numLevels, GL_RGBA8, width, height);

					for (unsigned i = 0; i < 6; i++)
					{
						glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image + (textureSize * i));
					}
				}

				if (numLevels > 1)
				{
					glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
				}

				target = GL_TEXTURE_CUBE_MAP;
			}
			else
			{
				unsigned char* image = static_cast<unsigned char*>(malloc(sizeof(unsigned char) * textureSize));
				defer{ free(image); };

				fread(image, sizeof(unsigned char), textureSize, fontFile);

				glGenTextures(1, &hTex);
				glBindTexture(GL_TEXTURE_2D, hTex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ResolveFilterMagMode(filter));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ResolveFilterMinMode(filter));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ResolveWrapMode(wrapModes.x));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ResolveWrapMode(wrapModes.y));
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropicLevel);

				unsigned numLevels = CountMipLevels(width, height, filter);
				if (CountChannels(format) == 3)
				{
					glTexStorage2D(GL_TEXTURE_2D, numLevels, GL_RGB8, width, height);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
				}
				else
				{
					glTexStorage2D(GL_TEXTURE_2D, numLevels, GL_RGBA8, width, height);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
				}

				if (numLevels > 1)
				{
					glGenerateMipmap(GL_TEXTURE_2D);
				}

				target = GL_TEXTURE_2D;
			}
		}
		else
		{
			auto image = Image::Load(filePath);
			if (image.data == nullptr)
				return false;

			width = image.width;
			height = image.height;
			format = image.format;

			glGenTextures(1, &hTex);
			glBindTexture(GL_TEXTURE_2D, hTex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ResolveFilterMagMode(filter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ResolveFilterMinMode(filter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ResolveWrapMode(wrapModes.x));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ResolveWrapMode(wrapModes.y));
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropicLevel);

			unsigned numLevels = CountMipLevels(width, height, filter);
			if (CountChannels(format) == 3)
			{
				glTexStorage2D(GL_TEXTURE_2D, numLevels, GL_RGB8, width, height);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.data);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, numLevels, GL_RGBA8, width, height);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
			}

			if (numLevels > 1)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}

			target = GL_TEXTURE_2D;
		}

		glBindTexture(target, GL_NONE);

		return true;
	}

	void Texture::SetFilterMode(TextureFilterMode _filter)
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");
		ASSERT(numSamples == 1, "It is illegal to change the filter mode of a multisampled texture.");

		glBindTexture(target, hTex);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, ResolveFilterMagMode(_filter));
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, ResolveFilterMinMode(_filter));
		glBindTexture(target, GL_NONE);

		filter = _filter;
	}

	void Texture::SetWrapModes(TextureWrapModes _wrapModes)
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");
		ASSERT(numSamples == 1, "It is illegal to change the wrap mode of a multisampled texture.");

		glBindTexture(target, hTex);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, ResolveWrapMode(_wrapModes.x));
		glTexParameteri(target, GL_TEXTURE_WRAP_T, ResolveWrapMode(_wrapModes.y));
		glBindTexture(target, GL_NONE);

		wrapModes = _wrapModes;
	}

	void Texture::SetAnisotropicLevel(float level)
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");
		ASSERT(level >= 1.0f && level <= 16.0f, "'level' must be in the range of [1, 16].");
		ASSERT(numSamples == 1, "It is illegal to change the anisotropic level of a multisampled texture.");

		glBindTexture(target, hTex);
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
		glBindTexture(target, GL_NONE);

		anisotropicLevel = level;
	}

	void Texture::Unload()
	{
		if (hTex != GL_NONE)
		{
			glDeleteTextures(1, &hTex);
			hTex = GL_NONE;
			target = GL_NONE;
		}
	}

	void Texture::Bind(unsigned slot)
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(target, hTex);
	}

	void Texture::UnBind(unsigned slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(target, GL_NONE);
	}

	unsigned Texture::GetHandle() const
	{
		return hTex;
	}

	unsigned Texture::GetNumSamples() const
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");

		return numSamples;
	}

	unsigned Texture::GetBindingTarget() const
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");

		return target;
	}

	unsigned Texture::GetWidth() const
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");

		return width;
	}

	unsigned Texture::GetHeight() const
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");

		return height;
	}

	TextureFormat Texture::GetTextureFormat() const
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");

		return format;
	}

	TextureFilterMode Texture::GetFilterMode() const
	{
		return filter;
	}

	TextureWrapModes Texture::GetWrapModes() const
	{
		return wrapModes;
	}

	float Texture::GetAnisotropicLevel() const
	{
		return anisotropicLevel;
	}

	bool Texture::IsCubeMap() const
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");

		return target == GL_TEXTURE_CUBE_MAP;
	}

	void Texture::RegenerateMipmaps()
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");
		ASSERT(numSamples == 1, "It is illegal to generate mipmaps on a multisampled texture.");

		glBindTexture(target, hTex);
		glGenerateMipmap(target);
		glBindTexture(target, GL_NONE);
	}

	//-----------------------------------------------------------------------------------------------------

	TextureSlot::TextureSlot(Texture::Ptr tex, unsigned unit)
		: tex(tex), unit(unit)
	{
	}

	void TextureSlot::Bind() const
	{
		tex->Bind(unit);
	}

	void TextureSlot::UnBind() const
	{
		tex->UnBind(unit);
	}

	//-----------------------------------------------------------------------------------------------------

	void TextureList::Bind() const
	{
		for (auto& slot : textureSlots)
		{
			slot.Bind();
		}
	}

	void TextureList::UnBind() const
	{
		for (auto& slot : textureSlots)
		{
			slot.UnBind();
		}
	}

	void TextureList::Add(Texture::Ptr tex, unsigned unit)
	{
		Remove(unit);

		textureSlots.push_back(TextureSlot(tex, unit));
	}

	void TextureList::Remove(unsigned unit)
	{
		for (auto itr = textureSlots.begin(); itr < textureSlots.end(); ++itr)
		{
			if (itr->unit == unit)
			{
				textureSlots.erase(itr);
				return;
			}
		}
	}

	void TextureList::Clear()
	{
		textureSlots.clear();
	}

	Texture::Ptr& TextureList::operator[](unsigned unit)
	{
		auto textureSlot = std::find_if(textureSlots.begin(), textureSlots.end(), [unit](TextureSlot& slot) {
			return slot.unit == unit;
		});

		ASSERT(textureSlot != textureSlots.end(), "No matching texture found.");
		return textureSlot->tex;
	}

	//-----------------------------------------------------------------------------------------------------
	
	Image::Image(int _width, int _height, TextureFormat _format, unsigned char* _data)
		: width(_width)
		, height(_height)
		, format(_format)
		, data(_data)
	{
	}

	Image::~Image()
	{
		free(const_cast<unsigned char*>(data));
	}

	Image Image::Load(const std::string& file, bool flipY)
	{
		int width = 0;
		int height = 0;
		int numChannels = 0;

		unsigned char* data = SOIL_load_image(file.c_str(), &width, &height, &numChannels, SOIL_LOAD_AUTO);
		if (data == nullptr)
		{
			Jwl::Error("Texture: ( %s )\n%s", file.c_str(), SOIL_last_result());
			return Image();
		}

		// Invert the Y axis for OpenGL texture addressing.
		if (flipY)
		{
			const int totalWidth = width * numChannels;
			for (int j = 0; j * 2 < height; ++j)
			{
				int index1 = j * totalWidth;
				int index2 = (height - 1 - j) * totalWidth;
				for (int i = totalWidth; i > 0; --i)
				{
					std::swap(data[index1++], data[index2++]);
				}
			}
		}

		if (numChannels == 3)
		{
			return Image(width, height, TextureFormat::RGB_8, data);
		}
		else if (numChannels == 4)
		{
			return Image(width, height, TextureFormat::RGBA_8, data);
		}
		else
		{
			Jwl::Error("Texture: ( %s )\nUnsupported format. Must have 3 or 4 color channels.", file.c_str());
			return Image();
		}
	}
}
