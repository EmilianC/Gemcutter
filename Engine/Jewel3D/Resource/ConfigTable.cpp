// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "ConfigTable.h"
#include "Jewel3D/Application/FileSystem.h"
#include "Jewel3D/Utilities/String.h"

namespace Jwl
{
	bool ConfigTable::Load(const std::string& file)
	{
		FileReader input;
		if (!input.OpenAsBuffer(file))
		{
			return false;
		}

		while (!input.IsEOF())
		{
			std::string line = input.GetLine();

			if (line.size() == 0 || line[0] == ':')
				continue;

			// Line must have an '=' to be processed.
			size_t pos = line.find("=");

			// Check that the position is valid and not the last character.
			if (pos == std::string::npos || pos == line.size() - 1)
				continue;

			std::string leftSide = line.substr(0, pos);
			std::string rightSide = line.substr(pos + 1);

			settings[leftSide] = rightSide;
		}

		return true;
	}

	bool ConfigTable::Save(const std::string& file) const
	{
		std::ofstream output(file);
		if (!output)
		{
			return false;
		}

		for (auto& [name, value] : settings)
		{
			output << name << "=" << value << '\n';
		}

		output.close();
		return true;
	}

	bool ConfigTable::operator==(const ConfigTable& other) const
	{
		return settings == other.settings;
	}

	bool ConfigTable::operator!=(const ConfigTable& other) const
	{
		return settings != other.settings;
	}

	bool ConfigTable::HasSetting(const std::string& setting) const
	{
		return settings.find(setting) != settings.end();
	}

	unsigned ConfigTable::GetSize() const
	{
		return settings.size();
	}

	std::string ConfigTable::GetString(const std::string& setting) const
	{
		std::string result;
		if (HasSetting(setting))
		{
			result = settings.at(setting);
		}

		return result;
	}

	float ConfigTable::GetFloat(const std::string& setting) const
	{
		return HasSetting(setting) ? std::stof(settings.at(setting)) : 0.0f;
	}

	int ConfigTable::GetInt(const std::string& setting) const
	{
		return HasSetting(setting) ? std::stoi(settings.at(setting)) : 0;
	}

	bool ConfigTable::GetBool(const std::string& setting) const
	{
		if (HasSetting(setting))
		{
			const auto& value = settings.at(setting);

			return
				value == "1" ||
				CompareLowercase(value, "on") || 
				CompareLowercase(value, "true");
		}
		else
		{
			return false;
		}
	}

	void ConfigTable::SetValue(const std::string& setting, const std::string& value)
	{
		settings[setting] = value;
	}

	void ConfigTable::SetValue(const std::string& setting, const char* value)
	{
		settings[setting] = value;
	}

	void ConfigTable::SetValue(const std::string& setting, float value)
	{
		settings[setting] = std::to_string(value);
	}

	void ConfigTable::SetValue(const std::string& setting, int value)
	{
		settings[setting] = std::to_string(value);
	}

	void ConfigTable::SetValue(const std::string& setting, bool value)
	{
		settings[setting] = value ? "on" : "off";
	}

	void ConfigTable::SetDefaultValue(const std::string& setting, const std::string& value)
	{
		if (!HasSetting(setting))
		{
			settings[setting] = value;
		}
	}

	void ConfigTable::SetDefaultValue(const std::string& setting, const char* value)
	{
		if (!HasSetting(setting))
		{
			settings[setting] = value;
		}
	}

	void ConfigTable::SetDefaultValue(const std::string& setting, float value)
	{
		if (!HasSetting(setting))
		{
			settings[setting] = std::to_string(value);
		}
	}

	void ConfigTable::SetDefaultValue(const std::string& setting, int value)
	{
		if (!HasSetting(setting))
		{
			settings[setting] = std::to_string(value);
		}
	}

	void ConfigTable::SetDefaultValue(const std::string& setting, bool value)
	{
		if (!HasSetting(setting))
		{
			settings[setting] = value ? "on" : "off";
		}
	}
}
