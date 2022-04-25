// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <cstdarg>
#include <string>
#include <string_view>

namespace gem
{
	// Removes all whitespace characters from the start of the string.
	void TrimStart(std::string& str);

	// Removes all whitespace characters from the end of the string.
	void TrimEnd(std::string& str);

	// Replaces the first occurrence of "from" in the source string with "to". Case sensitive.
	void ReplaceFirst(std::string& str, std::string_view from, std::string_view to);

	// Replaces the last occurrence of "from" in the source string with "to". Case sensitive.
	void ReplaceLast(std::string& str, std::string_view from, std::string_view to);

	// Replaces all occurrences of "from" in the source string with "to". Case sensitive.
	void ReplaceAll(std::string& str, std::string_view from, std::string_view to);

	// Remove all instances of whitespace from the string.
	void RemoveWhitespace(std::string& str);

	// Removes duplicate or unnecessary whitespace from the string based on standard programming syntax rules.
	// For Example "int   myVar,   temp,  *ptr ;" would become "int myVar, temp, *ptr;".
	void RemoveRedundantWhitespace(std::string& str);

	// Removes all instances of comments, "// ..." and "/* ... */", from the string.
	void RemoveComments(std::string& str);

	// Performs standard vsnprintf formatting on the argument list, up to 1024 characters.
	std::string FormatString(const char* format, ...);

	// Performs standard vsnprintf formatting on the argument list, up to 1024 characters.
	std::string FormatString(const char* format, va_list args);

	// Compares two strings for equality while ignoring capitalization.
	bool CompareLowercase(std::string_view, std::string_view);

	// Converts all uppercase letters in the string to lowercase.
	void ToLowercase(std::string& str);
}
