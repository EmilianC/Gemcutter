// Copyright (c) 2022 Emilian Cioca
#include "MaterialEncoder.h"
#include <gemcutter/Rendering/Rendering.h>

#define CURRENT_VERSION 2

MaterialEncoder::MaterialEncoder()
	: gem::Encoder(CURRENT_VERSION)
{
}

gem::ConfigTable MaterialEncoder::GetDefault() const
{
	gem::ConfigTable defaultConfig;

	defaultConfig.SetInt("version", CURRENT_VERSION);
	defaultConfig.SetString("shader", "");
	defaultConfig.SetString("texture_bind_points", "");
	defaultConfig.SetString("textures", "");
	defaultConfig.SetString("blend_mode", gem::EnumToString(gem::BlendFunc::None));
	defaultConfig.SetString("depth_mode", gem::EnumToString(gem::DepthFunc::Normal));
	defaultConfig.SetString("cull_mode",  gem::EnumToString(gem::CullFunc::Clockwise));

	return defaultConfig;
}

bool MaterialEncoder::Validate(const gem::ConfigTable& metadata, unsigned loadedVersion) const
{
	auto validateShader = [](const gem::ConfigTable& data)
	{
		if (!data.HasSetting("shader"))
		{
			gem::Error("Missing \"shader\" value.");
			return false;
		}

		std::string file = data.GetString("shader");
		if (!file.empty() && !gem::FileExists(file))
		{
			gem::Error("\"shader\" specifies a file ( %s ) that does not exist.", file.c_str());
			return false;
		}

		return true;
	};

	auto validateTextures = [](const gem::ConfigTable& data)
	{
		if (!data.HasSetting("texture_bind_points"))
		{
			gem::Error("Missing \"texture_bind_points\" array.");
			return false;
		}

		if (!data.HasSetting("textures"))
		{
			gem::Error("Missing \"textures\" array.");
			return false;
		}

		std::vector<int> units = data.GetIntArray("texture_bind_points");
		std::vector<std::string> textures = data.GetStringArray("textures");

		if (units.size() != textures.size())
		{
			gem::Error("\"texture_bind_points\" and \"textures\" must have the same number of elements.");
			return false;
		}

		for (unsigned i = 0; i < units.size(); ++i)
		{
			if (units[i] < 0)
			{
				gem::Error("\"texture_bind_points\" elements cannot be negative.");
				return false;
			}

			for (unsigned j = i + 1; j < units.size(); ++j)
			{
				if (units[i] == units[j])
				{
					gem::Error("\"texture_bind_points\" elements must all be unique.");
					return false;
				}
			}
		}

		for (const std::string& texture : textures)
		{
			if (!gem::FileExists(texture))
			{
				gem::Error("\"textures\" element refers to a file ( %s ) that does not exist.", texture.c_str());
				return false;
			}
		}

		return true;
	};

	auto validateBlendMode = [](const gem::ConfigTable& data, bool caseSensitive)
	{
		if (!data.HasSetting("blend_mode"))
		{
			gem::Error("Missing \"blend_mode\" value.");
			return false;
		}

		return gem::ValidateEnumValue<gem::BlendFunc>("blend_mode", data.GetString("blend_mode"), caseSensitive);
	};

	auto validateDepthMode = [](const gem::ConfigTable& data, bool caseSensitive)
	{
		if (!data.HasSetting("depth_mode"))
		{
			gem::Error("Missing \"depth_mode\" value.");
			return false;
		}

		return gem::ValidateEnumValue<gem::DepthFunc>("depth_mode", data.GetString("depth_mode"), caseSensitive);
	};

	auto validateCullMode = [](const gem::ConfigTable& data, bool caseSensitive)
	{
		if (!data.HasSetting("cull_mode"))
		{
			gem::Error("Missing \"cull_mode\" value.");
			return false;
		}

		return gem::ValidateEnumValue<gem::CullFunc>("cull_mode", data.GetString("cull_mode"), caseSensitive);
	};

	const bool caseSensitiveEnums = loadedVersion > 1;
	if (!validateShader(metadata)) return false;
	if (!validateTextures(metadata)) return false;
	if (!validateBlendMode(metadata, caseSensitiveEnums)) return false;
	if (!validateDepthMode(metadata, caseSensitiveEnums)) return false;
	if (!validateCullMode(metadata, caseSensitiveEnums)) return false;

	switch (loadedVersion)
	{
	case 1: [[fallthrough]];
	case 2:
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

bool MaterialEncoder::Convert(std::string_view source, std::string_view destination, const gem::ConfigTable& metadata) const
{
	const std::string outputFile = std::string(destination) + gem::ExtractFilename(source) + ".material";
	const std::string shader = metadata.GetString("shader");
	const std::vector<int> units = metadata.GetIntArray("texture_bind_points");
	const std::vector<std::string> textures = metadata.GetStringArray("textures");
	const auto BlendMode = gem::StringToEnum<gem::BlendFunc>(metadata.GetString("blend_mode")).value();
	const auto DepthMode = gem::StringToEnum<gem::DepthFunc>(metadata.GetString("depth_mode")).value();
	const auto cullMode  = gem::StringToEnum<gem::CullFunc>(metadata.GetString("cull_mode")).value();

	// Save file.
	FILE* materialFile = fopen(outputFile.c_str(), "wb");
	if (materialFile == nullptr)
	{
		gem::Error("Output file could not be created.");
		return false;
	}

	const size_t shaderLen = shader.size();
	fwrite(&shaderLen, sizeof(shaderLen), 1, materialFile);
	if (shaderLen)
	{
		fwrite(shader.c_str(), sizeof(char), shaderLen, materialFile);
	}

	const size_t textureCount = textures.size();
	fwrite(&textureCount, sizeof(textureCount), 1, materialFile);
	for (size_t i = 0; i < textureCount; ++i)
	{
		fwrite(&units[i], sizeof(units[0]), 1, materialFile);

		// Omit the file extension. It will be resolved when loading the texture.
		const char* start = textures[i].data();
		const char* end = strchr(textures[i].data(), '.');
		const size_t textureLen = end - start;

		fwrite(&textureLen, sizeof(textureLen), 1, materialFile);
		fwrite(textures[i].c_str(), sizeof(char), textureLen, materialFile);
	}

	fwrite(&BlendMode, sizeof(BlendMode), 1, materialFile);
	fwrite(&DepthMode, sizeof(DepthMode), 1, materialFile);
	fwrite(&cullMode, sizeof(cullMode), 1, materialFile);

	auto result = fclose(materialFile);
	if (result != 0)
	{
		gem::Error("Failed to generate Material Binary\nOutput file could not be saved.");
		return false;
	}

	return true;
}

bool MaterialEncoder::Upgrade(gem::ConfigTable& metadata, unsigned loadedVersion) const
{
	switch (loadedVersion)
	{
	case 1:
		// Enums became case sensitive.
		metadata.SetString("blend_mode", gem::FixEnumCasing(metadata.GetString("blend_mode"), gem::BlendFunc::None));
		metadata.SetString("depth_mode", gem::FixEnumCasing(metadata.GetString("depth_mode"), gem::DepthFunc::Normal));
		metadata.SetString("cull_mode",  gem::FixEnumCasing(metadata.GetString("cull_mode"),  gem::CullFunc::Clockwise));
		break;
	}

	return true;
}
