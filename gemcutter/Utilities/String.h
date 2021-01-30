// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <cstdarg>
#include <string>
#include <string_view>

namespace Jwl
{
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

	// Returns true if the first characters of base are equal to prefix.
	bool StartsWith(std::string_view base, std::string_view prefix);

	// Returns true if the last characters of base are equal to postfix.
	bool EndsWith(std::string_view base, std::string_view postfix);
}
