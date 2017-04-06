// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "ConfigTable.h"
#include "Jewel3D/Application/FileSystem.h"

#include <iostream>
#include <string>

// We don't use the Logger in this file because its output is intended to be processed by the AssetManager.

namespace Jwl
{
	//- A base class for all asset packers that provides a common interface to the AssetManager.
	//- Derive from this class to create a custom asset packing tool for integration with the AssetManager.
	class Encoder
	{
	public:
		Encoder(unsigned version)
			: version(version)
		{}
		virtual ~Encoder() = default;

	protected:
		//- The derived function should return default settings for the asset.
		virtual ConfigTable GetDefault() const = 0;

		//- The derived function should ensure that all fields are present and contain correct data.
		//- Validation should be done based on the provided version.
		virtual bool Validate(const ConfigTable& data, unsigned loadedVersion) const = 0;

		//- The derived function should read from the source file and output the packed asset to the destination file.
		//- data will contain settings of the newest version.
		virtual bool Convert(const std::string& source, const std::string& destination, const ConfigTable& data) const = 0;

		//- The derived function should update the provided data in order boost its version number by one.
		//- Upgrading is a sequential process, meaning that you only need to provide code to upgrade from 1-2 and from 2-3, not 1-3.
		virtual bool Upgrade(ConfigTable& data, unsigned loadedVersion) const { return true; };

	private:
		bool LoadData(const std::string& file);
		bool SaveData(const std::string& file) const;
		bool UpgradeData();
		bool ValidateData() const;

		//- Settings for packing the asset.
		ConfigTable metadata;

		// The newest version of the asset.
		const unsigned version;
	};

	__declspec(dllexport) bool __stdcall Reset(char* file)
	{
		Encoder e;

		if (FileExists(metaFile))
		{
			if (!RemoveFile(metaFile))
			{
				std::cout << "[e]Could not replace meta file. It might be in use by another process." << std::endl;
				return false;
			}
		}

		metadata = GetDefault();
		if (!SaveData(metaFile))
		{
			return false;
		}

		return true;
	}

	__declspec(dllexport) bool __stdcall Convert(char* src, char* dest)
	{
		Encoder e;

		std::string destination;
		if (!GetCommandLineArgument("-o", destination))
		{
			std::cout << "[e]No output file was specified." << std::endl;
			return false;
		}

		if (!LoadData(metaFile))
		{
			return false;
		}

		if (!Convert(AssetFile, destination, metadata))
		{
			return false;
		}

		return true;
	}

	__declspec(dllexport) bool __stdcall Update(char* file)
	{
		Encoder e;

		if (!LoadData(metaFile))
		{
			return false;
		}

		if (!UpgradeData())
		{
			return false;
		}

		if (!SaveData(metaFile))
		{
			return false;
		}

		return true;
	}

	__declspec(dllexport) bool __stdcall Validate(char* file)
	{
		Encoder e;

		if (!LoadData(metaFile))
		{
			return false;
		}

		return ValidateData();
	}
}
