// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "CmdArgs.h"

#include <stdlib.h>

namespace Jwl
{
	int GetArgc()
	{
		return __argc;
	}

	char** GetArgv()
	{
		return __argv;
	}

	bool ArgumentExists(const char* name)
	{
		for (int i = 1; i < __argc; i++)
		{
			// Search for the string.
			if (strcmp(__argv[i], name) == 0)
			{
				return true;
			}
		}

		return false;
	}

	int FindCommandLineArgument(const char* name)
	{
		for (int i = 1; i < __argc; i++)
		{
			// Search for the string.
			if (strcmp(__argv[i], name) == 0)
			{
				return i;
			}
		}

		return -1;
	}

	std::string GetExecutablePath()
	{
		return __argv[0];
	}

	bool GetCommandLineArgument(const char* name, bool& val)
	{
		int index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = atoi(__argv[index]) == 1;
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(const char* name, double& val)
	{
		int index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = atof(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(const char* name, float& val)
	{
		int index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = static_cast<float>(atof(__argv[index]));
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(const char* name, int& val)
	{
		int index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = atoi(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(const char* name, unsigned& val)
	{
		int index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = atoi(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(const char* name, char& val)
	{
		int index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = __argv[index][0];
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(const char* name, std::string& val)
	{
		int index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = __argv[index];
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(int index, bool& val)
	{
		if (index > 0 && index < __argc)
		{
			val = atoi(__argv[index]) == 1;
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(int index, double& val)
	{
		if (index > 0 && index < __argc)
		{
			val = atof(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(int index, float& val)
	{
		if (index > 0 && index < __argc)
		{
			val = static_cast<float>(atof(__argv[index]));
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(int index, int& val)
	{
		if (index > 0 && index < __argc)
		{
			val = atoi(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(int index, unsigned& val)
	{
		if (index > 0 && index < __argc)
		{
			val = atoi(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(int index, char& val)
	{
		if (index > 0 && index < __argc)
		{
			val = __argv[index][0];
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(int index, std::string& val)
	{
		// Index 0 is valid here. The first argument is always a string.
		if (index >= 0 && index < __argc)
		{
			val = __argv[index];
			return true;
		}

		return false;
	}
}
