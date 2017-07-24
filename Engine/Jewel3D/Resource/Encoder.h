// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "ConfigTable.h"
#include "Jewel3D/Application/FileSystem.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Utilities/ScopeGuard.h"

#include <memory>

namespace Jwl
{
	// A base class for all asset packers that exposes a common interface to the AssetManager.
	// Derive from this class to create a custom asset packing tool for integration with the AssetManager.
	class Encoder
	{
	public:
		Encoder(unsigned version)
			: version(version)
		{
		}
		virtual ~Encoder() = default;

		// Loads a .meta file and ensures that it has a valid version number.
		static bool LoadMetaData(const std::string& file, ConfigTable& out)
		{
			out = ConfigTable();

			if (ExtractFileExtension(file) != ".meta")
			{
				Error("Invalid file extension.");
				return false;
			}

			if (!out.Load(file))
			{
				Error("Failed to open metadata file.");
				return false;
			}

			if (!out.HasSetting("version"))
			{
				Error("Metadata file is missing a version specifier.");
				return false;
			}

			if (out.GetInt("version") < 1)
			{
				Error("Metadata version is invalid.");
				return false;
			}

			return true;
		}

		// Updates the metadata fields to the newest asset version.
		bool UpgradeData(ConfigTable& metadata) const
		{
			auto currentVersion = static_cast<unsigned>(metadata.GetInt("version"));

			if (currentVersion < version)
			{
				Log("Upgrading from version %d to %d.", currentVersion, version);

				// Sequentially upgrade the data to the latest version.
				while (currentVersion < version)
				{
					if (!Validate(metadata, currentVersion))
					{
						Error("Failed to validate metadata. Try resetting to defaults.");
						return false;
					}

					if (!Upgrade(metadata, currentVersion))
					{
						Error("Failed to update metadata. Try resetting to defaults.");
						return false;
					}

					currentVersion++;
					metadata.SetValue("version", static_cast<int>(currentVersion));
				}
			}

			if (!Validate(metadata, currentVersion))
			{
				Error("Failed to validate metadata. Try resetting to defaults.");
				return false;
			}

			return true;
		}

		bool ValidateData(const ConfigTable& metadata) const
		{
			return Validate(metadata, version);
		}

		// Returns the default settings for the asset.
		virtual ConfigTable GetDefault() const = 0;

		// Reads from the source file and output the packed asset to the destination file.
		// 'metadata' will contain settings of the newest version.
		virtual bool Convert(const std::string& source, const std::string& destination, const ConfigTable& metadata) const = 0;

	protected:
		// Ensures that all fields are present and contain valid data.
		// Validation should be done based on the provided version, which may not be the newest version.
		virtual bool Validate(const ConfigTable& metadata, unsigned loadedVersion) const = 0;

		// Updates the provided data in order boost its version number by one.
		// Upgrading is a sequential process, meaning that you only need to provide code to upgrade from 1->2 and from 2->3, not 1->3.
		virtual bool Upgrade(ConfigTable& metadata, unsigned loadedVersion) const { return true; };

	private:
		// The newest version of the asset.
		const unsigned version;
	};
}

// Must be implemented to return your derived Encoder.
std::unique_ptr<Jwl::Encoder> GetEncoder();

extern "C"
{
	// Convert command. Write the binary asset into the output folder.
	__declspec(dllexport) bool __cdecl Convert(const char* src, const char* dest)
	{
		defer { Jwl::ResetConsoleColor(); };
		auto encoder = GetEncoder();

		Jwl::ConfigTable metadata;
		if (!Jwl::Encoder::LoadMetaData(std::string(src) + ".meta", metadata))
		{
			return false;
		}

		if (!encoder->ValidateData(metadata))
		{
			return false;
		}

		if (!encoder->Convert(src, dest, metadata))
		{
			return false;
		}

		return true;
	}

	// Update command. Load the meta file and upgrade to the newest version.
	__declspec(dllexport) bool __cdecl Update(const char* file)
	{
		defer { Jwl::ResetConsoleColor(); };
		auto encoder = GetEncoder();

		const std::string metaFile = std::string(file) + ".meta";
		if (!Jwl::FileExists(metaFile))
		{
			auto metadata = encoder->GetDefault();
			if (!metadata.Save(metaFile))
			{
				Jwl::Error("Could create a new meta file.");
				return false;
			}

			if (!encoder->ValidateData(metadata))
			{
				return false;
			}

			return true;
		}

		Jwl::ConfigTable metadata;
		if (!Jwl::Encoder::LoadMetaData(metaFile, metadata))
		{
			return false;
		}

		Jwl::ConfigTable upgradedData = metadata;
		if (!encoder->UpgradeData(upgradedData))
		{
			return false;
		}

		// Avoid touching the file if it is not changed.
		if (metadata != upgradedData)
		{
			if (!upgradedData.Save(metaFile))
			{
				return false;
			}
		}

		return true;
	}
}
