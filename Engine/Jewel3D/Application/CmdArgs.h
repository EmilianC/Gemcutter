// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <string>

#include "Jewel3D/Application/Types.h"

/*
* Assists in reading command line arguments, especially in the format:
* -<argument name> <argument value>
* 
* For example, if the command line arguments are:
* -width 10 -name MyGame -enableDebug 1 -speed 10.2
* 
* Then we could retrieve these values with:
* GetCommandLineArgument("-width", &s32)
* GetCommandLineArgument("-name", &string)
* GetCommandLineArgument("-enableDebug", &bool)
* GetCommandLineArgument("-speed", &f32)
*/

namespace Jwl
{
	s32 GetArgc();
	char** GetArgv();

	//- Returns true if the command line argument with the given name is defined.
	bool ArgumentExists(const char* name);
	//- Returns the index of an argument or -1 if the argument is not found.
	s32 FindCommandLineArgument(const char* name);
	//- Returns the directory in which the program's executable was run.
	std::string GetExecutablePath();

	//- Gets the entry after the specified argument name.
	bool GetCommandLineArgument(const char* name, bool& val);
	bool GetCommandLineArgument(const char* name, f64& val);
	bool GetCommandLineArgument(const char* name, f32& val);
	bool GetCommandLineArgument(const char* name, s32& val);
	bool GetCommandLineArgument(const char* name, u32& val);
	bool GetCommandLineArgument(const char* name, s8& val);
	bool GetCommandLineArgument(const char* name, std::string& val);

	//- Gets the entry at the specified index.
	bool GetCommandLineArgument(s32 index, bool& val);
	bool GetCommandLineArgument(s32 index, f64& val);
	bool GetCommandLineArgument(s32 index, f32& val);
	bool GetCommandLineArgument(s32 index, s32& val);
	bool GetCommandLineArgument(s32 index, u32& val);
	bool GetCommandLineArgument(s32 index, s8& val);
	bool GetCommandLineArgument(s32 index, std::string& val);
}
