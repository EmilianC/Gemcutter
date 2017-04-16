// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "ConfigTable.h"
#include "Jewel3D/Application/FileSystem.h"

#include <iostream>
#include <memory>
#include <string>

// We don't use the Logger in this file because output is intended to be processed by the AssetManager instead.

namespace Jwl
{
	//- A base class for all asset packers that provides a common interface to the AssetManager.
	//- Derive from this class to create a custom asset packing tool for integration with the AssetManager.
	class Encoder
	{
	public:
		Encoder(unsigned version)
			: version(version)
		{
		}
		virtual ~Encoder() = default;

		// 
		static ConfigTable LoadMetaData(const std::string& file)
		{
			ConfigTable result;

			if (Jwl::ExtractFileExtension(file) != ".meta")
			{
				std::cout << "[e]Invalid file extension." << std::endl;
				return result;
			}

			if (!result.Load(file))
			{
				std::cout << "[e]Failed to open metadata file." << std::endl;
				return result;
			}

			if (!result.HasSetting("version"))
			{
				std::cout << "[e]Metadata file is missing a version specifier." << std::endl;
				return result;
			}

			if (result.GetInt("version") < 1)
			{
				std::cout << "[e]Metadata version is invalid." << std::endl;
				return result;
			}

			return result;
		}

		bool UpgradeData(ConfigTable& data) const
		{
			// Sequentially upgrade the data to the latest version.
			auto currentVersion = static_cast<unsigned>(data.GetInt("version"));
			while (currentVersion < version)
			{
				if (!Validate(data, currentVersion))
				{
					std::cout << "[e]Failed to validate metadata. Try resetting to defaults." << std::endl;
					return false;
				}

				if (!Upgrade(data, currentVersion))
				{
					std::cout << "[e]Failed to update metadata. Try resetting to defaults." << std::endl;
					return false;
				}

				currentVersion++;
				data.SetValue("version", static_cast<int>(currentVersion));
			}

			if (!Validate(data, currentVersion))
			{
				std::cout << "[e]Failed to validate metadata. Try resetting to defaults." << std::endl;
				return false;
			}

			return true;
		}

		bool ValidateData(const ConfigTable& data) const
		{
			return Validate(data, version);
		}

		//- The derived function should return default settings for the asset.
		virtual ConfigTable GetDefault() const = 0;

		//- The derived function should read from the source file and output the packed asset to the destination file.
		//- data will contain settings of the newest version.
		virtual bool Convert(const std::string& source, const std::string& destination, const ConfigTable& data) const = 0;

	protected:
		//- The derived function should ensure that all fields are present and contain correct data.
		//- Validation should be done based on the provided version.
		virtual bool Validate(const ConfigTable& data, unsigned loadedVersion) const = 0;

		//- The derived function should update the provided data in order boost its version number by one.
		//- Upgrading is a sequential process, meaning that you only need to provide code to upgrade from 1->2 and from 2->3, not 1->3.
		virtual bool Upgrade(ConfigTable& data, unsigned loadedVersion) const { return true; };

	private:
		// The newest version of the asset.
		const unsigned version;
	};
}

// Must be implemented to return your derived Encoder.
std::unique_ptr<Jwl::Encoder> GetEncoder();

// Reset command. Replace the metadata with a default configuration.
extern "C" __declspec(dllexport) bool __cdecl Initialize(char* file)
{
	auto encoder = GetEncoder();

	if (Jwl::ExtractFileExtension(file) != ".meta")
	{
		std::cout << "[e]Invalid file extension." << std::endl;
		return false;
	}

	if (!Jwl::FileExists(file))
	{
		auto metadata = encoder->GetDefault();
		if (metadata.Save(file))
		{
			std::cout << "[e]Could create a new meta file." << std::endl;
			return false;
		}
	}
	
	return true;
}

// Convert command. Write the binary asset into the output folder.
extern "C" __declspec(dllexport) bool __cdecl Convert(char* src, char* dest)
{
	auto encoder = GetEncoder();

	auto metadata = Jwl::Encoder::LoadMetaData(Jwl::ExtractPath(src) + Jwl::ExtractFilename(src) + ".meta");
	if (metadata.GetSize() == 0)
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
extern "C" __declspec(dllexport) bool __cdecl Update(char* file)
{
	auto encoder = GetEncoder();

	auto metadata = Jwl::Encoder::LoadMetaData(file);
	if (metadata.GetSize() == 0)
	{
		return false;
	}

	if (!encoder->UpgradeData(metadata))
	{
		return false;
	}

	if (!metadata.Save(file))
	{
		return false;
	}

	return true;
}

// Validate command. Ensure the metaData is valid.
extern "C" __declspec(dllexport) bool __cdecl Validate(char* file)
{
	auto encoder = GetEncoder();

	auto metadata = Jwl::Encoder::LoadMetaData(file);
	if (metadata.GetSize() == 0)
	{
		return false;
	}

	return encoder->ValidateData(metadata);
}
