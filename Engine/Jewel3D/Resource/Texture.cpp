// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Texture.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Math/Math.h"
#include "Jewel3D/Utilities/ScopeGuard.h"

#include <GLEW/GL/glew.h>
#include <SOIL/SOIL.h>
#include <algorithm>

namespace Jwl
{
	TextureFilterMode Texture::defaultFilterMode = TextureFilterMode::Trilinear;
	float Texture::defaultAnisotropicLevel = 1.0f;

	Texture::~Texture()
	{
		Unload();
	}

	void Texture::CreateTexture(unsigned _width, unsigned _height, TextureFormat _format, TextureFilterMode filter, unsigned _numSamples)
	{
		ASSERT(hTex == 0, "Texture already has a texture loaded.");
		ASSERT(_numSamples == 1 || _numSamples == 2 || _numSamples == 4 || _numSamples == 8 || _numSamples == 16, "'numSamples' must be a power of 2 between 1 and 16.");

		width = static_cast<int>(_width);
		height = static_cast<int>(_height);
		numChannels = ResolveChannels(_format);
		numSamples = _numSamples;
		format = _format;

		glGenTextures(1, &hTex);

		int numLevels = 1;
		if (ResolveMipMapping(filter))
		{
			float max = static_cast<float>(Max(width, height));
			numLevels = static_cast<int>(std::floor(std::log2(max))) + 1;
		}

		if (numSamples == 1)
		{
			glBindTexture(GL_TEXTURE_2D, hTex);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ResolveFilterMagMode(filter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ResolveFilterMinMode(filter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, defaultAnisotropicLevel);

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

	bool Texture::Load(const std::string& filePath, TextureFilterMode filter)
	{
		ASSERT(hTex == 0, "Texture already has a texture loaded.");

		unsigned char* image = SOIL_load_image(filePath.c_str(), &width, &height, &numChannels, SOIL_LOAD_AUTO);

		if (image == nullptr)
		{
			Error("Texture: ( %s )\n%s", filePath.c_str(), SOIL_last_result());
			return false;
		}

		defer {
			SOIL_free_image_data(image);
		};

		// Invert the Y axis.
		for (int j = 0; j * 2 < height; ++j)
		{
			int index1 = j * width * numChannels;
			int index2 = (height - 1 - j) * width * numChannels;
			for (int i = width * numChannels; i > 0; --i)
			{
				unsigned char temp = image[index1];
				image[index1] = image[index2];
				image[index2] = temp;
				++index1;
				++index2;
			}
		}

		glGenTextures(1, &hTex);
		glBindTexture(GL_TEXTURE_2D, hTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ResolveFilterMagMode(filter));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ResolveFilterMinMode(filter));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, defaultAnisotropicLevel);

		int numLevels = 1;
		if (ResolveMipMapping(filter))
		{
			float max = static_cast<float>(Max(width, height));
			numLevels = static_cast<int>(std::floor(std::log2(max))) + 1;
		}

		if (numChannels == 3)
		{
			// Send the texture data.
			glTexStorage2D(GL_TEXTURE_2D, numLevels, GL_RGB8, width, height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
			format = TextureFormat::RGB_8;
		}
		else if (numChannels == 4)
		{
			// Send the texture data.
			glTexStorage2D(GL_TEXTURE_2D, numLevels, GL_RGBA8, width, height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
			format = TextureFormat::RGBA_8;
		}
		else
		{
			Error("Texture: ( %s )\nUnsupported format. Must have 3 or 4 color channels.", filePath.c_str());
			return false;
		}

		if (numLevels > 1)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		target = GL_TEXTURE_2D;
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		return true;
	}

	bool Texture::LoadCubeMap(
		const std::string& filePathPosX, const std::string& filePathNegX,
		const std::string& filePathPosY, const std::string& filePathNegY,
		const std::string& filePathPosZ, const std::string& filePathNegZ,
		TextureFilterMode filter)
	{
		ASSERT(hTex == 0, "Texture already has a texture loaded.");

		// Used to compare and make sure every cubemap face is identical.
		int widthRef = 0;
		int heightRef = 0;
		int channelRef = 0;

		unsigned char* faces[6] = { nullptr };
		const std::string* files[6] = { &filePathPosX, &filePathNegX, &filePathPosY, &filePathNegY, &filePathPosZ, &filePathNegZ };

		defer {
			for (unsigned i = 0; i < 6; i++)
			{
				if (faces[i]) SOIL_free_image_data(faces[i]);
			}
		};

		faces[0] = SOIL_load_image(filePathPosX.c_str(), &width, &height, &numChannels, SOIL_LOAD_AUTO);
		if (faces[0] == nullptr)
		{
			Error("Texture: ( %s )\n%s", filePathPosX.c_str(), SOIL_last_result());
			return false;
		}
		else if (width != height)
		{
			Error("Texture: ( %s )\nCubemap faces must be square (width = height).", filePathPosX.c_str());
			return false;
		}

		for (unsigned i = 1; i < 6; i++)
		{
			faces[i] = SOIL_load_image(files[i]->c_str(), &widthRef, &heightRef, &channelRef, SOIL_LOAD_AUTO);
			if (faces[i] == nullptr)
			{
				Error("Texture: ( %s )\n%s", files[i]->c_str(), SOIL_last_result());
				return false;
			}
			else if (width != widthRef || height != heightRef || numChannels != channelRef)
			{
				Error("Texture: ( %s )\nHas a different size or format than other cubemap faces.", files[i]->c_str());
				return false;
			}
			else if (widthRef != heightRef)
			{
				Error("Texture: ( %s )\nCubemap faces must be square (width = height).", files[i]->c_str());
				return false;
			}
		}

		glGenTextures(1, &hTex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, hTex);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, ResolveFilterMagMode(filter));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, ResolveFilterMinMode(filter));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		int numLevels;
		if (ResolveMipMapping(filter))
		{
			float max = static_cast<float>(Max(width, height));
			numLevels = static_cast<int>(std::floor(std::log2(max))) + 1;
		}
		else
		{
			numLevels = 1;
		}

		if (numChannels == 3)
		{
			format = TextureFormat::RGB_8;
			glTexStorage2D(GL_TEXTURE_CUBE_MAP, numLevels, GL_RGB8, width, height);

			for (unsigned i = 0; i < 6; i++)
			{
				glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, faces[i]);
			}
		}
		else if (numChannels == 4)
		{
			format = TextureFormat::RGBA_8;
			glTexStorage2D(GL_TEXTURE_CUBE_MAP, numLevels, GL_RGBA8, width, height);

			for (unsigned i = 0; i < 6; i++)
			{
				glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, faces[i]);
			}
		}
		else
		{
			Error("Texture: ( %s )\nUnsupported format. Must have 3 or 4 color channels.", filePathPosX.c_str());
			return false;
		}

		if (numLevels > 1)
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}

		target = GL_TEXTURE_CUBE_MAP;
		glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);

		return true;
	}

	void Texture::SetWrapMode(TextureWrapMode wrap)
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");
		ASSERT(numSamples == 1, "It is illegal to change the wrap mode of a multisampled texture.");

		glBindTexture(target, hTex);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, ResolveWrapMode(wrap));
		glTexParameteri(target, GL_TEXTURE_WRAP_T, ResolveWrapMode(wrap));
		glTexParameteri(target, GL_TEXTURE_WRAP_R, ResolveWrapMode(wrap));
		glBindTexture(target, GL_NONE);
	}

	void Texture::SetWrapModeS(TextureWrapMode wrap)
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");
		ASSERT(numSamples == 1, "It is illegal to change the wrap mode of a multisampled texture.");

		glBindTexture(target, hTex);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, ResolveWrapMode(wrap));
		glBindTexture(target, GL_NONE);
	}

	void Texture::SetWrapModeT(TextureWrapMode wrap)
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");
		ASSERT(numSamples == 1, "It is illegal to change the wrap mode of a multisampled texture.");

		glBindTexture(target, hTex);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, ResolveWrapMode(wrap));
		glBindTexture(target, GL_NONE);
	}

	void Texture::SetWrapModeR(TextureWrapMode wrap)
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");
		ASSERT(numSamples == 1, "It is illegal to change the wrap mode of a multisampled texture.");

		glBindTexture(target, hTex);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, ResolveWrapMode(wrap));
		glBindTexture(target, GL_NONE);
	}

	void Texture::SetFilterMode(TextureFilterMode filter)
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");
		ASSERT(numSamples == 1, "It is illegal to change the filter mode of a multisampled texture.");

		glBindTexture(target, hTex);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, ResolveFilterMagMode(filter));
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, ResolveFilterMinMode(filter));
		glBindTexture(target, GL_NONE);
	}

	void Texture::SetFilterModeMin(TextureFilterMode filter)
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");
		ASSERT(numSamples == 1, "It is illegal to change the filter mode of a multisampled texture.");

		glBindTexture(target, hTex);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, ResolveFilterMinMode(filter));
		glBindTexture(target, GL_NONE);
	}

	void Texture::SetFilterModeMag(TextureFilterMode filter)
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");
		ASSERT(numSamples == 1, "It is illegal to change the filter mode of a multisampled texture.");

		glBindTexture(target, hTex);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, ResolveFilterMagMode(filter));
		glBindTexture(target, GL_NONE);
	}

	void Texture::SetAnisotropicLevel(float level)
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");
		ASSERT(level >= 1.0f && level <= 16.0f, "'level' must be in the range of [1, 16].");
		ASSERT(numSamples == 1, "It is illegal to change the anisotropic level of a multisampled texture.");

		glBindTexture(target, hTex);
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
		glBindTexture(target, GL_NONE);
	}

	void Texture::Unload()
	{
		if (hTex != GL_NONE)
		{
			// Remove data stored with OpenGL.
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

	unsigned Texture::GetNumChannels() const
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");

		return numChannels;
	}

	unsigned Texture::GetNumSamples() const
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");

		return numSamples;
	}

	TextureFormat Texture::GetTextureFormat() const
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");

		return format;
	}

	unsigned Texture::GetBindingTarget() const
	{
		ASSERT(hTex != 0, "A texture must be loaded to call this function.");

		return target;
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

	void Texture::SetDefaultFilterMode(TextureFilterMode filterMode)
	{
		defaultFilterMode = filterMode;
	}

	void Texture::SetDefaultAnisotropicLevel(float level)
	{
		ASSERT(level >= 1.0f && level <= 16.0f, "'level' must be in the range of [1, 16].");

		defaultAnisotropicLevel = level;
	}

	TextureFilterMode Texture::GetDefaultFilterMode()
	{
		return defaultFilterMode;
	}

	float Texture::GetDefaultAnisotropicLevel()
	{
		return defaultAnisotropicLevel;
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
		for (auto itr = textureSlots.begin(); itr < textureSlots.end(); itr++)
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
}
