// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Encoder.h"
#include "Jewel3D/Application/CmdArgs.h"
#include "Jewel3D/Application/FileSystem.h"

#include <iostream>

namespace Jwl
{
	int Encoder::ProcessCommands()
	{
		// Fine the specified asset file.
		std::string AssetFile;
		std::string metaFile;
		if (!GetCommandLineArgument("-i", AssetFile))
		{
			std::cout << "[e]No asset file was specified." << std::endl;
			return EXIT_FAILURE;
		}

		if (!FileExists(AssetFile))
		{
			std::cout << "[e]Asset file does not exist." << std::endl;
			return EXIT_FAILURE;
		}

		auto pos = AssetFile.find_last_of('.');
		if (pos == std::string::npos)
		{
			std::cout << "[e]Asset file did not have an extension." << std::endl;
			return EXIT_FAILURE;
		}

		metaFile = AssetFile.substr(0, pos);
		metaFile += ".meta";

		// Reset command. Replace the metadata with a default configuration.
		if (FindCommandLineArgument("-r") != -1)
		{
			
		}

		// Convert command. Write the binary asset into the output folder.
		if (FindCommandLineArgument("-c") != -1)
		{
			
		}

		// Update command. Load the meta file and upgrade to the newest version.
		if (FindCommandLineArgument("-u") != -1)
		{
			
		}

		// Validate command. Ensure the metaData is valid.
		if (FindCommandLineArgument("-v") != -1)
		{
			
		}

		return EXIT_SUCCESS;
	}

	bool Encoder::LoadData(const std::string& file)
	{
		if (!metadata.Load(file))
		{
			std::cout << "[e]Failed to open metadata file." << std::endl;
			return false;
		}

		if (!metadata.HasSetting("version"))
		{
			std::cout << "[e]Metadata file is missing a version specifier." << std::endl;
			return false;
		}

		if (metadata.GetInt("version") < 1)
		{
			std::cout << "[e]Metadata version is invalid." << std::endl;
			return false;
		}

		return true;
	}

	bool Encoder::SaveData(const std::string& file) const
	{
		if (!metadata.Save(file))
		{
			std::cout << "[e]Could not replace meta file. It might be in use by another process." << std::endl;
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	bool Encoder::UpgradeData()
	{
		// Sequentially upgrade the data to the latest version.
		auto currentVersion = static_cast<unsigned>(metadata.GetInt("version"));
		while (currentVersion < version)
		{
			if (!Validate(metadata, currentVersion))
			{
				std::cout << "[e]Failed to validate metadata. Try resetting to defaults." << std::endl;
				return false;
			}

			if (!Upgrade(metadata, currentVersion))
			{
				std::cout << "[e]Failed to update metadata. Try resetting to defaults." << std::endl;
				return false;
			}

			currentVersion++;
			metadata.SetValue("version", static_cast<int>(currentVersion));
		}

		if (!Validate(metadata, currentVersion))
		{
			std::cout << "[e]Failed to validate metadata. Try resetting to defaults." << std::endl;
			return false;
		}

		return true;
	}

	bool Encoder::ValidateData() const
	{
		return Validate(metadata, version);
	}
}
