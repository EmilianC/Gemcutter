// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <string>

/*
* Assists in reading command line arguments, especially in the format of
* -<argument name> <argument value>
* 
* For example, if the command line arguments are
* -width 10 -name MyGame -enableDebug 1 -speed 10.2
* 
* Then we could retrieve these values with
* GetCommandLineArgument("-width", &int)
* GetCommandLineArgument("-name", &string)
* GetCommandLineArgument("-enableDebug", &bool)
* GetCommandLineArgument("-speed", &float)
*/

namespace Jwl
{
	int GetArgc();
	char** GetArgv();

	bool ArgumentExists(const char* name);
	//- Returns an index to an argument or -1 if the argument is not found.
	int FindCommandLineArgument(const char* name);
	//- Returns the directory in which the program's executable was run.
	std::string GetExecutablePath();

	//- Gets the entry after the specified argument name
	bool GetCommandLineArgument(const char* name, bool& val);
	bool GetCommandLineArgument(const char* name, double& val);
	bool GetCommandLineArgument(const char* name, float& val);
	bool GetCommandLineArgument(const char* name, int& val);
	bool GetCommandLineArgument(const char* name, unsigned& val);
	bool GetCommandLineArgument(const char* name, char& val);
	bool GetCommandLineArgument(const char* name, std::string& val);

	//- Gets the entry at the specified index
	bool GetCommandLineArgument(int index, bool& val);
	bool GetCommandLineArgument(int index, double& val);
	bool GetCommandLineArgument(int index, float& val);
	bool GetCommandLineArgument(int index, int& val);
	bool GetCommandLineArgument(int index, unsigned& val);
	bool GetCommandLineArgument(int index, char& val);
	bool GetCommandLineArgument(int index, std::string& val);
}
