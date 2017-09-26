// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <stdarg.h>
#include <string>

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

	// Compares two strings for equality while ignore capitalization.
	bool CompareLowercase(const std::string&, const std::string&);

	// Returns true if the first characters of base are equal to start.
	bool StartsWith(const std::string& base, const std::string& start);

	// Returns true if the last characters of base are equal to end.
	bool EndsWith(const std::string& base, const std::string& end);
}
