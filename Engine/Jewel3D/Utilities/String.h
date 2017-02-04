// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <stdarg.h>
#include <string>

/*
* Various Utility functions relating to string manipulation.
*/

namespace Jwl
{
	//- Remove all instances of whitespace from the string.
	void RemoveWhitespace(std::string& str);

	//- Removes duplicate or unnecessary whitespace from a string based on standard programming syntax rules.
	//- For Example "int   myVar,   temp,  *ptr ;" would become "int myVar, temp, *ptr;".
	void RemoveRedundantWhitespace(std::string& str);
	
	//- Performs standard vsnprintf formating on the argument list, up to 1024 characters.
	std::string FormatString(const char* format, ...);

	//- Performs standard vsnprintf formating on the argument list, up to 1024 characters.
	std::string FormatString(const char* format, va_list args);
}
