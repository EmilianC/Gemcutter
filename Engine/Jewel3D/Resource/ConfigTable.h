// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <string>
#include <unordered_map>

namespace Jwl
{
	class ConfigTable
	{
	public:
		bool LoadConfig(const std::string& file);
		bool SaveConfig(const std::string& file) const;

		bool HasSetting(const std::string& setting) const;

		std::string GetString(const std::string& setting) const;
		float GetFloat(const std::string& setting) const;
		int GetInt(const std::string& setting) const;
		bool GetBool(const std::string& setting) const;

		void SetValue(const std::string& setting, const std::string& value);
		void SetValue(const std::string& setting, float value);
		void SetValue(const std::string& setting, int value);
		void SetValue(const std::string& setting, bool value);

		//- Does not override an existing setting, but creates it if it doesn't exist.
		void SetDefaultValue(const std::string& setting, const std::string& value);
		//- Does not override an existing setting, but creates it if it doesn't exist.
		void SetDefaultValue(const std::string& setting, float value);
		//- Does not override an existing setting, but creates it if it doesn't exist.
		void SetDefaultValue(const std::string& setting, int value);
		//- Does not override an existing setting, but creates it if it doesn't exist.
		void SetDefaultValue(const std::string& setting, bool value);

	private:
		std::unordered_map<std::string, std::string> settings;
	};
}
