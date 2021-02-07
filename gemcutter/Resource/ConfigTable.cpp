// Copyright (c) 2017 Emilian Cioca
#include "ConfigTable.h"
#include "gemcutter/Application/FileSystem.h"
#include "gemcutter/Utilities/String.h"

namespace
{
	template<typename Func>
	void ProcessArrayElements(std::string_view array, Func&& functor)
	{
		if (array.empty())
			return;

		const char* currentPos = array.data();
		const char* currentElementStart = currentPos;
		const char* end = currentPos + array.size();
		while (currentPos != end)
		{
			if (*currentPos == ',')
			{
				functor(std::string_view(currentElementStart, currentPos - currentElementStart));
				currentElementStart = currentPos + 1;
			}

			++currentPos;
		}

		functor(std::string_view(currentElementStart, end - currentElementStart));
	}
}

namespace Jwl
{
	bool ConfigTable::Load(std::string_view file)
	{
		FileReader input;
		if (!input.OpenAsBuffer(file))
		{
			return false;
		}

		while (!input.IsEOF())
		{
			std::string line = input.GetLine();

			if (line.empty() || line[0] == ':')
				continue;

			size_t pos = line.find('=');
			if (pos == std::string::npos)
				continue;

			std::string leftSide = line.substr(0, pos);
			std::string rightSide;
			if (pos != line.size() - 1)
			{
				rightSide = line.substr(pos + 1);
			}

			settings[std::move(leftSide)] = std::move(rightSide);
		}

		return true;
	}

	bool ConfigTable::Save(std::string_view file) const
	{
		std::ofstream output(file.data());
		if (!output)
		{
			return false;
		}

		for (auto& [name, value] : settings)
		{
			output << name << '=' << value << '\n';
		}

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

	bool ConfigTable::HasSetting(std::string_view setting) const
	{
		return settings.count(setting);
	}

	unsigned ConfigTable::GetSize() const
	{
		return settings.size();
	}

	std::string ConfigTable::GetString(std::string_view setting) const
	{
		std::string result;
		auto itr = settings.find(setting);

		if (itr != settings.end())
		{
			result = itr->second;
		}

		return result;
	}

	float ConfigTable::GetFloat(std::string_view setting) const
	{
		auto itr = settings.find(setting);
		if (itr != settings.end())
		{
			return std::stof(itr->second);
		}
		else
		{
			return 0.0f;
		}
	}

	int ConfigTable::GetInt(std::string_view setting) const
	{
		auto itr = settings.find(setting);
		if (itr != settings.end())
		{
			return std::stoi(itr->second);
		}
		else
		{
			return 0;
		}
	}

	bool ConfigTable::GetBool(std::string_view setting) const
	{
		auto itr = settings.find(setting);
		if (itr != settings.end())
		{
			const auto& value = itr->second;

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

	std::vector<std::string> ConfigTable::GetStringArray(std::string_view setting) const
	{
		std::vector<std::string> results;

		auto itr = settings.find(setting);
		if (itr != settings.end())
		{
			ProcessArrayElements(itr->second, [&](std::string_view element) {
				results.emplace_back(element.data(), element.size());
			});
		}

		return results;
	}

	std::vector<float> ConfigTable::GetFloatArray(std::string_view setting) const
	{
		std::vector<float> results;

		auto itr = settings.find(setting);
		if (itr != settings.end())
		{
			ProcessArrayElements(itr->second, [&](std::string_view element) {
				results.push_back(std::stof(element.data()));
			});
		}

		return results;
	}

	std::vector<int> ConfigTable::GetIntArray(std::string_view setting) const
	{
		std::vector<int> results;

		auto itr = settings.find(setting);
		if (itr != settings.end())
		{
			ProcessArrayElements(itr->second, [&](std::string_view element) {
				results.push_back(std::stoi(element.data()));
			});
		}

		return results;
	}

	std::vector<bool> ConfigTable::GetBoolArray(std::string_view setting) const
	{
		std::vector<bool> results;

		auto itr = settings.find(setting);
		if (itr != settings.end())
		{
			ProcessArrayElements(itr->second, [&](std::string_view element) {
				results.push_back(
					element[0] == '1' ||
					CompareLowercase(element, "on") ||
					CompareLowercase(element, "true"));
			});
		}

		return results;
	}

	void ConfigTable::SetValue(const std::string& setting, std::string_view value)
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

	void ConfigTable::SetDefaultValue(const std::string& setting, std::string_view value)
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
