// Copyright (c) 2022 Emilian Cioca
#include "TextureEncoder.h"
#include <gemcutter/Rendering/Rendering.h>
#include <gemcutter/Resource/Texture.h>
#include <gemcutter/Utilities/String.h>

#define CURRENT_VERSION 3

TextureEncoder::TextureEncoder()
	: gem::Encoder(CURRENT_VERSION)
{
}

gem::ConfigTable TextureEncoder::GetDefault() const
{
	gem::ConfigTable defaultConfig;

	defaultConfig.SetInt("version", CURRENT_VERSION);
	defaultConfig.SetFloat("anisotropic_level", 1.0f);
	defaultConfig.SetBool("cubemap", false);
	defaultConfig.SetBool("s_rgb", true);
	defaultConfig.SetString("filter", gem::EnumToString(gem::TextureFilter::Linear));
	defaultConfig.SetString("wrap_x", gem::EnumToString(gem::TextureWrap::Clamp));
	defaultConfig.SetString("wrap_y", gem::EnumToString(gem::TextureWrap::Clamp));

	return defaultConfig;
}

bool TextureEncoder::Validate(const gem::ConfigTable& metadata, unsigned loadedVersion) const
{
	auto validateAnisotropicLevel = [](const gem::ConfigTable& data)
	{
		if (!data.HasSetting("anisotropic_level"))
		{
			gem::Error("Missing \"anisotropic_level\" value.");
			return false;
		}

		float anisotropicLevel = data.GetFloat("anisotropic_level");
		if (anisotropicLevel < 1.0f || anisotropicLevel > 16.0f)
		{
			gem::Error("\"anisotropic_level\" must be in the range of [1, 16].");
			return false;
		}

		return true;
	};

	auto validateTextureFilter = [](const gem::ConfigTable& data, bool caseSensitive)
	{
		if (!data.HasSetting("filter"))
		{
			gem::Error("Missing \"filter\" value.");
			return false;
		}

		return gem::ValidateEnumValue<gem::TextureFilter>("filter", data.GetString("filter"), caseSensitive);
	};

	auto validateWrap = [](const gem::ConfigTable& data, std::string_view mode, bool caseSensitive)
	{
		if (!data.HasSetting(mode))
		{
			gem::Error("Missing \"%s\" value.", mode.data());
			return false;
		}

		return gem::ValidateEnumValue<gem::TextureWrap>(mode, data.GetString(mode), caseSensitive);
	};

	if (!metadata.HasSetting("cubemap"))
	{
		gem::Error("Missing \"cubemap\" value.");
		return false;
	}

	const bool caseSensitiveEnums = loadedVersion > 2;
	if (!validateAnisotropicLevel(metadata)) return false;
	if (!validateTextureFilter(metadata, caseSensitiveEnums)) return false;
	if (!validateWrap(metadata, "wrap_x", caseSensitiveEnums)) return false;
	if (!validateWrap(metadata, "wrap_y", caseSensitiveEnums)) return false;

	switch (loadedVersion)
	{
	case 1:
		if (metadata.GetSize() != 6)
		{
			gem::Error("Incorrect number of value entries.");
			return false;
		}
		break;

	case 2: [[fallthrough]];
	case 3:
		if (!metadata.HasSetting("s_rgb"))
		{
			gem::Error("Missing \"s_rgb\" value.");
			return false;
		}

		if (metadata.GetSize() != 7)
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

bool TextureEncoder::Convert(std::string_view source, std::string_view destination, const gem::ConfigTable& metadata) const
{
	const std::string outputFile = std::string(destination) + gem::ExtractFilename(source) + ".texture";
	const float anisotropicLevel = metadata.GetFloat("anisotropic_level");
	const bool isCubemap = metadata.GetBool("cubemap");
	const bool isSRGB = metadata.GetBool("s_rgb");
	const auto filter = gem::StringToEnum<gem::TextureFilter>(metadata.GetString("filter")).value();
	const auto wrapX  = gem::StringToEnum<gem::TextureWrap>(metadata.GetString("wrap_x")).value();
	const auto wrapY  = gem::StringToEnum<gem::TextureWrap>(metadata.GetString("wrap_y")).value();

	auto image = gem::RawImage::Load(source, !isCubemap, isSRGB);
	if (image.data == nullptr)
		return false;

	const unsigned elementCount = gem::CountChannels(image.format);

	// Save file.
	FILE* textureFile = fopen(outputFile.c_str(), "wb");
	if (textureFile == nullptr)
	{
		gem::Error("Output file could not be created.");
		return false;
	}

	if (isCubemap)
	{
		// Validate size. Width to height ratio must be 4/3.
		if (image.width * 3 != image.height * 4)
		{
			fclose(textureFile);
			gem::Error("Cubemap texture layout is incorrect.");
			return false;
		}

		if (wrapX != gem::TextureWrap::Clamp ||
			wrapY != gem::TextureWrap::Clamp)
		{
			gem::Warning("Cubemaps must have \"Clamp\" wrap modes. Forcing wrap modes to \"Clamp\".");
		}

		const unsigned faceSize = image.width / 4;
		const gem::TextureWrap cubeMapWrapMode = gem::TextureWrap::Clamp;

		// Write header.
		fwrite(&isCubemap, sizeof(bool), 1, textureFile);
		fwrite(&faceSize, sizeof(unsigned), 1, textureFile);
		fwrite(&faceSize, sizeof(unsigned), 1, textureFile);
		fwrite(&image.format, sizeof(gem::TextureFormat), 1, textureFile);
		fwrite(&filter, sizeof(gem::TextureFilter), 1, textureFile);
		fwrite(&cubeMapWrapMode, sizeof(gem::TextureWrap), 1, textureFile);
		fwrite(&cubeMapWrapMode, sizeof(gem::TextureWrap), 1, textureFile);
		fwrite(&anisotropicLevel, sizeof(float), 1, textureFile);

		// Write faces.
		const unsigned textureSize = faceSize * faceSize * elementCount;
		unsigned char* data = static_cast<unsigned char*>(malloc(sizeof(unsigned char) * textureSize * 6));
		defer { free(data); };

		// Strip out the dead space of the source texture, the final data is 6 textures in sequence.
		auto copyFace = [&image, faceSize, elementCount](unsigned char* dest, unsigned startX, unsigned startY)
		{
			for (unsigned y = startY; y < startY + faceSize; ++y)
			{
				for (unsigned x = startX; x < startX + faceSize; ++x)
				{
					unsigned index = (x + y * image.width) * elementCount;

					memcpy(dest, image.data + index, elementCount);
					dest += elementCount;
				}
			}
		};

		copyFace(data, faceSize * 2, faceSize);						// +X
		copyFace(data + 1 * textureSize, 0, faceSize);				// -X
		copyFace(data + 2 * textureSize, faceSize, 0);				// +Y
		copyFace(data + 3 * textureSize, faceSize, faceSize * 2);	// -Y
		copyFace(data + 4 * textureSize, faceSize, faceSize);		// +Z
		copyFace(data + 5 * textureSize, faceSize * 3, faceSize);	// -Z

		fwrite(data, sizeof(unsigned char), textureSize * 6, textureFile);
	}
	else
	{
		// Write header.
		fwrite(&isCubemap, sizeof(bool), 1, textureFile);
		fwrite(&image.width, sizeof(unsigned), 1, textureFile);
		fwrite(&image.height, sizeof(unsigned), 1, textureFile);
		fwrite(&image.format, sizeof(gem::TextureFormat), 1, textureFile);
		fwrite(&filter, sizeof(gem::TextureFilter), 1, textureFile);
		fwrite(&wrapX, sizeof(gem::TextureWrap), 1, textureFile);
		fwrite(&wrapY, sizeof(gem::TextureWrap), 1, textureFile);
		fwrite(&anisotropicLevel, sizeof(float), 1, textureFile);

		// Write Data.
		const unsigned textureSize = image.width * image.height * elementCount;
		fwrite(image.data, sizeof(unsigned char), textureSize, textureFile);
	}

	auto result = fclose(textureFile);
	if (result != 0)
	{
		gem::Error("Failed to generate Texture Binary\nOutput file could not be saved.");
		return false;
	}

	return true;
}

bool TextureEncoder::Upgrade(gem::ConfigTable& metadata, unsigned loadedVersion) const
{
	switch (loadedVersion)
	{
	case 1:
		// Added s_rgb field.
		metadata.SetBool("s_rgb", true);
		break;

	case 2:
		// Enums became case sensitive.
		metadata.SetString("filter", gem::FixEnumCasing(metadata.GetString("filter"), gem::TextureFilter::Linear));
		metadata.SetString("wrap_x", gem::FixEnumCasing(metadata.GetString("wrap_x"), gem::TextureWrap::Clamp));
		metadata.SetString("wrap_y", gem::FixEnumCasing(metadata.GetString("wrap_y"), gem::TextureWrap::Clamp));
		break;
	}

	return true;
}
