// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <string>
#include <map>

#include "Jewel3D/Application/Types.h"

namespace Jwl
{
	class ConfigTable
	{
	public:
		bool Load(const std::string& file);
		bool Save(const std::string& file) const;

		bool operator==(const ConfigTable&) const;
		bool operator!=(const ConfigTable&) const;

		//- Returns true if the setting has a value in the table.
		bool HasSetting(const std::string& setting) const;
		//- Returns the number of entries in the table.
		u32 GetSize() const;

		std::string GetString(const std::string& setting) const;
		f32 GetFloat(const std::string& setting) const;
		s32 GetInt(const std::string& setting) const;
		bool GetBool(const std::string& setting) const;

		//- Creates or overwrites an existing setting.
		void SetValue(const std::string& setting, const std::string& value);
		void SetValue(const std::string& setting, const char* value);
		void SetValue(const std::string& setting, f32 value);
		void SetValue(const std::string& setting, s32 value);
		void SetValue(const std::string& setting, bool value);

		//- Does not overwrite an existing setting, but creates it if it doesn't exist.
		void SetDefaultValue(const std::string& setting, const std::string& value);
		void SetDefaultValue(const std::string& setting, const char* value);
		void SetDefaultValue(const std::string& setting, f32 value);
		void SetDefaultValue(const std::string& setting, s32 value);
		void SetDefaultValue(const std::string& setting, bool value);

	private:
		std::map<std::string, std::string> settings;
	};
}
