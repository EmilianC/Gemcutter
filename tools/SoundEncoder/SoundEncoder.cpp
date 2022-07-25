// Copyright (c) 2022 Emilian Cioca
#include "SoundEncoder.h"
#include <gemcutter/Resource/Sound.h>
#include <gemcutter/Utilities/String.h>

#define CURRENT_VERSION 1

SoundEncoder::SoundEncoder()
	: gem::Encoder(CURRENT_VERSION)
{
}

gem::ConfigTable SoundEncoder::GetDefault() const
{
	gem::ConfigTable defaultConfig;

	defaultConfig.SetInt("version", CURRENT_VERSION);
	defaultConfig.SetBool("3d", false);
	defaultConfig.SetBool("loop", false);
	defaultConfig.SetBool("unique_instance", false);
	defaultConfig.SetString("3d_attenuation", "none");
	defaultConfig.SetFloat("3d_attenuation_rolloff", 1.0f);
	defaultConfig.SetFloat("volume", 1.0f);
	defaultConfig.SetFloat("3d_min_distance", 1.0f);
	defaultConfig.SetFloat("3d_max_distance", 100.0f);

	return defaultConfig;
}

bool SoundEncoder::Validate(const gem::ConfigTable& metadata, unsigned loadedVersion) const
{
	if (!metadata.HasSetting("3d"))
	{
		gem::Error("Missing \"3d\" value.");
		return false;
	}

	if (!metadata.HasSetting("loop"))
	{
		gem::Error("Missing \"loop\" value.");
		return false;
	}

	if (!metadata.HasSetting("unique_instance"))
	{
		gem::Error("Missing \"unique_instance\" value.");
		return false;
	}

	if (!metadata.HasSetting("3d_attenuation"))
	{
		gem::Error("Missing \"3d_attenuation\" value.");
		return false;
	}

	const std::string attenuation = metadata.GetString("3d_attenuation");
	if (!gem::CompareLowercase(attenuation, "none") &&
		!gem::CompareLowercase(attenuation, "inversedistance") &&
		!gem::CompareLowercase(attenuation, "linear") &&
		!gem::CompareLowercase(attenuation, "exponential"))
	{
		gem::Error("\"3d_attenuation\" is invalid. Valid options are \"none\", \"inversedistance\", \"linear\", or \"exponential\".");
		return false;
	}

	if (!metadata.HasSetting("3d_attenuation_rolloff"))
	{
		gem::Error("Missing \"3d_attenuation_rolloff\" value.");
		return false;
	}

	const float rolloff = metadata.GetFloat("3d_attenuation_rolloff");
	if (rolloff < 0.0f)
	{
		gem::Error("\"3d_attenuation_rolloff\" cannot be less than zero.");
		return false;
	}

	if (gem::CompareLowercase(attenuation, "linear") && rolloff > 1.0f)
	{
		gem::Error("\"3d_attenuation_rolloff\" cannot be greater than 1 when using linear \"3d_attenuation\".");
		return false;
	}

	if (!metadata.HasSetting("volume"))
	{
		gem::Error("Missing \"volume\" value.");
		return false;
	}

	const float volume = metadata.GetFloat("volume");
	if (volume < 0.0f)
	{
		gem::Error("\"volume\" cannot be less than zero.");
		return false;
	}

	if (!metadata.HasSetting("3d_min_distance"))
	{
		gem::Error("Missing \"3d_min_distance\" value.");
		return false;
	}

	if (!metadata.HasSetting("3d_max_distance"))
	{
		gem::Error("Missing \"3d_max_distance\" value.");
		return false;
	}

	const float minDistance = metadata.GetFloat("3d_min_distance");
	const float maxDistance = metadata.GetFloat("3d_max_distance");
	if (minDistance <= 0.0f)
	{
		gem::Error("\"3d_min_distance\" must be greater than zero.");
		return false;
	}

	if (maxDistance < minDistance)
	{
		gem::Error("\"3d_max_distance\" cannot be less than \"3d_min_distance\".");
		return false;
	}

	switch (loadedVersion)
	{
	case 1:
		if (metadata.GetSize() != 9)
		{
			gem::Error("Incorrect number of value entries.");
			return false;
		}
		break;
	}

	return true;
}

bool SoundEncoder::Convert(std::string_view source, std::string_view destination, const gem::ConfigTable& metadata) const
{
	const std::string outputFile = std::string(destination) + gem::ExtractFilename(source) + ".sound";
	const bool is3D = metadata.GetBool("3d");
	const bool loop = metadata.GetBool("loop");
	const bool unique = metadata.GetBool("unique_instance");
	const gem::AttenuationFunc attenuation = gem::StringToAttenuationFunc(metadata.GetString("3d_attenuation"));
	const float rolloff = metadata.GetFloat("3d_attenuation_rolloff");
	const float volume = metadata.GetFloat("volume");
	const float minDistance = metadata.GetFloat("3d_min_distance");
	const float maxDistance = metadata.GetFloat("3d_max_distance");

	FILE* soundFile = fopen(outputFile.c_str(), "wb");
	if (soundFile == nullptr)
	{
		gem::Error("Output file could not be created.");
		return false;
	}

	fwrite(&is3D, sizeof(bool), 1, soundFile);
	fwrite(&loop, sizeof(bool), 1, soundFile);
	fwrite(&unique, sizeof(bool), 1, soundFile);
	fwrite(&attenuation, sizeof(gem::AttenuationFunc), 1, soundFile);
	fwrite(&rolloff, sizeof(float), 1, soundFile);
	fwrite(&volume, sizeof(float), 1, soundFile);
	fwrite(&minDistance, sizeof(float), 1, soundFile);
	fwrite(&maxDistance, sizeof(float), 1, soundFile);

	std::vector<std::byte> rawSoundFile;
	if (!gem::LoadFileAsBinary(source, rawSoundFile))
	{
		gem::Error("Could not read the source file ( %s ).", source.data());
		return false;
	}

	const size_t size = rawSoundFile.size();
	fwrite(&size, sizeof(size_t), 1, soundFile);
	fwrite(rawSoundFile.data(), size, 1, soundFile);

	auto result = fclose(soundFile);
	if (result != 0)
	{
		gem::Error("Failed to generate Sound Binary\nOutput file could not be saved.");
		return false;
	}

	return true;
}