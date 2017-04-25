// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <string>
#include <map>

namespace Jwl
{
	class ConfigTable
	{
	public:
		bool Load(const std::string& file);
		bool Save(const std::string& file) const;

		//- Returns true if the setting has a value in the table.
		bool HasSetting(const std::string& setting) const;
		//- Returns the number of entries in the table.
		unsigned GetSize() const;

		std::string GetString(const std::string& setting) const;
		float GetFloat(const std::string& setting) const;
		int GetInt(const std::string& setting) const;
		bool GetBool(const std::string& setting) const;

		//- Creates or overwrites an existing setting.
		void SetValue(const std::string& setting, const std::string& value);
		void SetValue(const std::string& setting, float value);
		void SetValue(const std::string& setting, int value);
		void SetValue(const std::string& setting, bool value);

		//- Does not overwrite an existing setting, but creates it if it doesn't exist.
		void SetDefaultValue(const std::string& setting, const std::string& value);
		void SetDefaultValue(const std::string& setting, float value);
		void SetDefaultValue(const std::string& setting, int value);
		void SetDefaultValue(const std::string& setting, bool value);

	private:
		std::map<std::string, std::string> settings;
	};
}
