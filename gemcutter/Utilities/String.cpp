// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "String.h"

#include <algorithm>
#include <cctype>

namespace
{
	constexpr unsigned BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE] = { '\0' };
}

namespace Jwl
{
	void RemoveWhitespace(std::string& str)
	{
		str.erase(std::remove_if(str.begin(), str.end(), [](char x) {
			return std::isspace(x);
		}), str.end());
	}

	void RemoveRedundantWhitespace(std::string& str)
	{
		bool skipTabs = false;

		for (auto itr = str.begin(); itr < str.end() - 1; ++itr)
		{
			char lhs = *itr;
			char rhs = *(itr + 1);

			if (skipTabs)
			{
				if (lhs == '\t')
				{
					continue;
				}
				else
				{
					skipTabs = false;
				}
			}

			// Subsequent tabs are allowed at the start of a line.
			if (lhs == '\n' && rhs == '\t')
			{
				skipTabs = true;
				continue;
			}

			// Never delete a single newline character.
			if ((lhs == '\n' && rhs != '\n') ||
				(lhs != '\n' && rhs == '\n'))
			{
				continue;
			}

			if (std::isspace(lhs) && std::isspace(rhs))
			{
				str.erase(itr--);
			}
		}
	}

	void RemoveComments(std::string& str)
	{
		// Remove comments starting with '//'.
		size_t pos = str.find("//");
		while (pos != std::string::npos)
		{
			size_t endl = str.find('\n', pos);
			str.erase(pos, (endl - pos) + 1);

			pos = str.find("//", pos);
		}

		// Remove multi-line comments.
		pos = str.find("/*");
		while (pos != std::string::npos)
		{
			size_t endl = str.find("*/", pos);
			str.erase(pos, (endl - pos) + 2);

			pos = str.find("/*", pos);
		}
	}

	std::string FormatString(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		std::string result = FormatString(format, argptr);
		va_end(argptr);

		return result;
	}

	std::string FormatString(const char* format, va_list args)
	{
		std::string result;

		if (vsnprintf(buffer, BUFFER_SIZE, format, args) >= 0)
		{
			result = buffer;
		}

		return result;
	}

	bool CompareLowercase(std::string_view a, std::string_view b)
	{
		if (a.size() != b.size())
		{
			return false;
		}

		for (unsigned i = 0; i < a.size(); ++i)
		{
			if (std::tolower(a[i]) != std::tolower(b[i]))
			{
				return false;
			}
		}

		return true;
	}

	void ToLowercase(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(),
			[](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
	}

	bool StartsWith(std::string_view base, std::string_view prefix)
	{
		if (base.size() < prefix.size())
		{
			return false;
		}

		return strncmp(base.data(), prefix.data(), prefix.size()) == 0;
	}

	bool EndsWith(std::string_view base, std::string_view postfix)
	{
		if (base.size() < postfix.size())
		{
			return false;
		}

		return strncmp((base.data() + base.size()) - postfix.size(), postfix.data(), postfix.size()) == 0;
	}
}
