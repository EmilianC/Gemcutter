// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <string>

/*
 Assists in reading command line arguments, especially in the format:
 -<argument name> <argument value>

 For example, if the command line arguments are:
 -width 10 -name MyGame -enableDebug 1 -speed 10.2

 Then we could retrieve these values with:
 GetCommandLineArg("-width", &int)
 GetCommandLineArg("-name", &string)
 GetCommandLineArg("-enableDebug", &bool)
 GetCommandLineArg("-speed", &float)
*/

namespace Jwl
{
	int GetArgc();
	char** GetArgv();

	// Returns true if the command line argument with the given name exists.
	bool HasCommandLineArg(const char* name);
	// Returns the index of an argument or -1 if the argument is not found.
	int FindCommandLineArg(const char* name);
	// Returns the directory in which the program's executable was run.
	const char* GetExecutablePath();

	// Gets the argument at the specified index.
	bool GetCommandLineArg(int index, bool& value);
	bool GetCommandLineArg(int index, double& value);
	bool GetCommandLineArg(int index, float& value);
	bool GetCommandLineArg(int index, int& value);
	bool GetCommandLineArg(int index, unsigned& value);
	bool GetCommandLineArg(int index, char& value);
	bool GetCommandLineArg(int index, const char*& value);
	bool GetCommandLineArg(int index, std::string& value);

	// Gets the argument value after the specified name.
	template<typename T>
	bool GetCommandLineArg(const char* name, T& value)
	{
		int index = FindCommandLineArg(name);
		if (index == -1)
		{
			return false;
		}

		return GetCommandLineArg(index + 1, value);
	}
}
