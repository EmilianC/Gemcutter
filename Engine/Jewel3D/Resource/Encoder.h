// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "ConfigTable.h"

#include <string>

namespace Jwl
{
	//- A base class for all asset packers that provides a common interface to the AssetManager.
	//- Derive from this class to create a custom asset packing tool for integration with the AssetManager.
	class Encoder
	{
	public:
		Encoder(unsigned version);
		virtual ~Encoder() = default;

		//- Reads the command line arguments from the AssetManager and calls the appropriate virtual functions.
		int ProcessCommands();

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

		//- Settings to packing the asset.
		ConfigTable metadata;

		// The newest version of the asset.
		const unsigned version;
	};
}