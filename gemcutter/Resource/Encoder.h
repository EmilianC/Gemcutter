// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Application/CmdArgs.h"
#include "gemcutter/Application/FileSystem.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Resource/ConfigTable.h"
#include "gemcutter/Utilities/ScopeGuard.h"
#include "gemcutter/Utilities/String.h"

#include <memory>

namespace gem
{
	// A base class for all asset packers that exposes a common interface to the AssetManager.
	// Derive from this class to create a custom asset packing tool for integration with the AssetManager.
	class Encoder
	{
	public:
		Encoder(unsigned version) : version(version) {}
		virtual ~Encoder() = default;

		// Loads a .meta file and ensures that it has a valid version number.
		static bool LoadMetaData(std::string_view file, ConfigTable& out)
		{
			out = ConfigTable();

			if (!CompareLowercase(ExtractFileExtension(file), ".meta"))
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

		// Parses the command line arguments of the program and invokes the user-provided Encoder.
		static bool RunEncoder(Encoder& encoder)
		{
			const char* usage =
				"Gemcutter asset Encoder.\nUsage:\n"
				"  Encoder.exe -update -src <file>\n"
				"  Encoder.exe -pack -src <file> -dest <folder>\n"
				"Options:\n"
				"  -pack       Package the file into the destination folder.\n"
				"  -update     Ensures that the asset's metadata file is up to date.";

			if (HasCommandLineArg("-pack"))
			{
				const char* src = nullptr;
				if (!GetCommandLineArg("-src", src))
				{
					Error("Invalid command line parameters: Missing '-src <file>'");
					Log(usage);
					return false;
				}

				const char* dest = nullptr;
				if (!GetCommandLineArg("-dest", dest))
				{
					Error("Invalid command line parameters: Missing '-dest <folder>'");
					Log(usage);
					return false;
				}

				return encoder.Pack(src, dest);
			}
			else if (HasCommandLineArg("-update"))
			{
				const char* src = nullptr;
				if (!GetCommandLineArg("-src", src))
				{
					Error("Invalid command line parameters: Missing '-src <file>'");
					Log(usage);
					return false;
				}

				return encoder.Update(src);
			}

			Log(usage);
			return false;
		}

		bool ValidateData(const ConfigTable& metadata) const
		{
			return Validate(metadata, version);
		}

		// Update command. Load the meta file and upgrade to the newest version.
		bool Update(const char* file)
		{
			defer { ResetConsoleColor(); };

			const std::string metaFile = std::string(file) + ".meta";
			if (!FileExists(metaFile))
			{
				auto metadata = GetDefault();
				if (!metadata.Save(metaFile))
				{
					Error("Could create a new meta file.");
					return false;
				}

				if (!ValidateData(metadata))
				{
					return false;
				}

				return true;
			}

			ConfigTable metadata;
			if (!Encoder::LoadMetaData(metaFile, metadata))
			{
				return false;
			}

			ConfigTable upgradedData = metadata;
			if (!UpgradeData(upgradedData))
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

		// Pack command. Write the binary asset into the output folder.
		bool Pack(const char* src, const char* dest)
		{
			defer { ResetConsoleColor(); };

			ConfigTable metadata;
			if (!Encoder::LoadMetaData(std::string(src) + ".meta", metadata))
			{
				return false;
			}

			if (!ValidateData(metadata))
			{
				return false;
			}

			if (!Convert(src, dest, metadata))
			{
				return false;
			}

			return true;
		}

		// Returns the default settings for the asset.
		virtual ConfigTable GetDefault() const = 0;

	protected:
		// Reads from the source file and output the packed asset to the destination file.
		// 'metadata' will contain settings of the newest version.
		virtual bool Convert(std::string_view source, std::string_view destination, const ConfigTable& metadata) const = 0;

		// Ensures that all fields are present and contain valid data.
		// Validation should be done based on the provided version, which may not be the newest version.
		virtual bool Validate(const ConfigTable& metadata, unsigned loadedVersion) const = 0;

		// Updates the provided data in order boost its version number by one.
		// Upgrading is a sequential process, meaning that you only need to provide code to upgrade from 1->2 and from 2->3, not 1->3.
		virtual bool Upgrade(ConfigTable& metadata, unsigned loadedVersion) const { return true; };

		// The newest version of the metaData.
		const unsigned version;
	};
}
