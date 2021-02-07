// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <string>
#include <string_view>
#include <map>
#include <vector>

namespace gem
{
	class ConfigTable
	{
	public:
		bool Load(std::string_view file);
		bool Save(std::string_view file) const;

		bool operator==(const ConfigTable&) const;
		bool operator!=(const ConfigTable&) const;

		// Returns true if the setting has a value in the table.
		bool HasSetting(std::string_view setting) const;
		// Returns the number of entries in the table.
		unsigned GetSize() const;

		std::string GetString(std::string_view setting) const;
		float GetFloat(std::string_view setting) const;
		int GetInt(std::string_view setting) const;
		bool GetBool(std::string_view setting) const;

		std::vector<std::string> GetStringArray(std::string_view setting) const;
		std::vector<float> GetFloatArray(std::string_view setting) const;
		std::vector<int> GetIntArray(std::string_view setting) const;
		std::vector<bool> GetBoolArray(std::string_view setting) const;

		// Creates or overwrites an existing setting.
		void SetValue(const std::string& setting, std::string_view value);
		void SetValue(const std::string& setting, const char* value);
		void SetValue(const std::string& setting, float value);
		void SetValue(const std::string& setting, int value);
		void SetValue(const std::string& setting, bool value);

		// Does not overwrite an existing setting, but creates it if it doesn't exist.
		void SetDefaultValue(const std::string& setting, std::string_view value);
		void SetDefaultValue(const std::string& setting, const char* value);
		void SetDefaultValue(const std::string& setting, float value);
		void SetDefaultValue(const std::string& setting, int value);
		void SetDefaultValue(const std::string& setting, bool value);

	private:
		std::map<std::string, std::string, std::less<>> settings;
	};
}
