#include "MaterialEncoder.h"
#include <gemcutter/Rendering/Rendering.h>

#define CURRENT_VERSION 1

MaterialEncoder::MaterialEncoder()
	: gem::Encoder(CURRENT_VERSION)
{
}

gem::ConfigTable MaterialEncoder::GetDefault() const
{
	gem::ConfigTable defaultConfig;

	defaultConfig.SetValue("version", CURRENT_VERSION);
	defaultConfig.SetValue("shader", "");
	defaultConfig.SetValue("texture_bind_points", "");
	defaultConfig.SetValue("textures", "");
	defaultConfig.SetValue("blend_mode", "none");
	defaultConfig.SetValue("depth_mode", "normal");
	defaultConfig.SetValue("cull_mode", "clockwise");

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

		for (std::string& texture : textures)
		{
			if (!gem::FileExists(texture))
			{
				gem::Error("\"textures\" element refers to a file ( %s ) that does not exist.", texture.c_str());
				return false;
			}
		}

		return true;
	};

	auto validateBlendMode = [](const gem::ConfigTable& data)
	{
		if (!data.HasSetting("blend_mode"))
		{
			gem::Error("Missing \"blend_mode\" value.");
			return false;
		}

		auto str = data.GetString("blend_mode");
		if (!gem::CompareLowercase(str, "none") &&
			!gem::CompareLowercase(str, "linear") &&
			!gem::CompareLowercase(str, "additive") &&
			!gem::CompareLowercase(str, "multiplicative"))
		{
			gem::Error("\"blend_mode\" is invalid. Valid options are \"none\", \"linear\", \"additive\", or \"multiplicative\".");
			return false;
		}

		return true;
	};

	auto validateDepthMode = [](const gem::ConfigTable& data)
	{
		if (!data.HasSetting("depth_mode"))
		{
			gem::Error("Missing \"depth_mode\" value.");
			return false;
		}

		auto str = data.GetString("depth_mode");
		if (!gem::CompareLowercase(str, "none") &&
			!gem::CompareLowercase(str, "writeonly") &&
			!gem::CompareLowercase(str, "testonly") &&
			!gem::CompareLowercase(str, "normal"))
		{
			gem::Error("\"depth_mode\" is invalid. Valid options are \"none\", \"writeonly\", \"testonly\", or \"normal\".");
			return false;
		}

		return true;
	};

	auto validateCullMode = [](const gem::ConfigTable& data)
	{
		if (!data.HasSetting("cull_mode"))
		{
			gem::Error("Missing \"cull_mode\" value.");
			return false;
		}

		auto str = data.GetString("cull_mode");
		if (!gem::CompareLowercase(str, "none") &&
			!gem::CompareLowercase(str, "clockwise") &&
			!gem::CompareLowercase(str, "counterclockwise"))
		{
			gem::Error("\"cull_mode\" is invalid. Valid options are \"none\", \"clockwise\", or \"counterclockwise\".");
			return false;
		}

		return true;
	};

	if (!validateShader(metadata)) return false;
	if (!validateTextures(metadata)) return false;
	if (!validateBlendMode(metadata)) return false;
	if (!validateDepthMode(metadata)) return false;
	if (!validateCullMode(metadata)) return false;

	switch (loadedVersion)
	{
	case 1:
		if (metadata.GetSize() != 7)
		{
			gem::Error("Incorrect number of value entries.");
			return false;
		}
	}

	return true;
}

bool MaterialEncoder::Convert(std::string_view source, std::string_view destination, const gem::ConfigTable& metadata) const
{
	const std::string outputFile = std::string(destination) + gem::ExtractFilename(source) + ".material";
	const std::string shader = metadata.GetString("shader");
	const std::vector<int> units = metadata.GetIntArray("texture_bind_points");
	const std::vector<std::string> textures = metadata.GetStringArray("textures");
	const gem::BlendFunc BlendMode = gem::StringToBlendFunc(metadata.GetString("blend_mode"));
	const gem::DepthFunc DepthMode = gem::StringToDepthFunc(metadata.GetString("depth_mode"));
	const gem::CullFunc cullMode = gem::StringToCullFunc(metadata.GetString("cull_mode"));

	// Save file.
	FILE* materialFile = fopen(outputFile.c_str(), "wb");
	if (materialFile == nullptr)
	{
		gem::Error("Output file could not be created.");
		return false;
	}

	const size_t shaderLen = shader.size();
	fwrite(&shaderLen, sizeof(size_t), 1, materialFile);
	if (shaderLen)
	{
		fwrite(shader.c_str(), sizeof(char), shaderLen, materialFile);
	}

	const size_t textureCount = textures.size();
	fwrite(&textureCount, sizeof(size_t), 1, materialFile);
	for (size_t i = 0; i < textureCount; ++i)
	{
		fwrite(&units[i], sizeof(int), 1, materialFile);

		// Omit the file extension. It will be resolved when loading the texture.
		const char* start = textures[i].data();
		const char* end = strchr(textures[i].data(), '.');
		const size_t textureLen = end - start;

		fwrite(&textureLen, sizeof(size_t), 1, materialFile);
		fwrite(textures[i].c_str(), sizeof(char), textureLen, materialFile);
	}

	fwrite(&BlendMode, sizeof(gem::BlendFunc), 1, materialFile);
	fwrite(&DepthMode, sizeof(gem::DepthFunc), 1, materialFile);
	fwrite(&cullMode, sizeof(gem::CullFunc), 1, materialFile);

	auto result = fclose(materialFile);
	if (result != 0)
	{
		gem::Error("Failed to generate Material Binary\nOutput file could not be saved.");
		return false;
	}

	return true;
}
