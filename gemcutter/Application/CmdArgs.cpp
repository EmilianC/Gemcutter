// Copyright (c) 2017 Emilian Cioca
#include "CmdArgs.h"

#include <cstdlib>

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

	bool HasCommandLineArg(const char* name)
	{
		return FindCommandLineArg(name) != -1;
	}

	int FindCommandLineArg(const char* name)
	{
		for (int i = 1; i < __argc; ++i)
		{
			if (strcmp(__argv[i], name) == 0)
			{
				return i;
			}
		}

		return -1;
	}

	const char* GetExecutablePath()
	{
		return __argv[0];
	}

	bool GetCommandLineArg(int index, bool& value)
	{
		if (index > 0 && index < __argc)
		{
			value = atoi(__argv[index]) == 1;
			return true;
		}

		return false;
	}

	bool GetCommandLineArg(int index, double& value)
	{
		if (index > 0 && index < __argc)
		{
			value = atof(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArg(int index, float& value)
	{
		if (index > 0 && index < __argc)
		{
			value = static_cast<float>(atof(__argv[index]));
			return true;
		}

		return false;
	}

	bool GetCommandLineArg(int index, int& value)
	{
		if (index > 0 && index < __argc)
		{
			value = atoi(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArg(int index, unsigned& value)
	{
		if (index > 0 && index < __argc)
		{
			value = atoi(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArg(int index, char& value)
	{
		if (index > 0 && index < __argc)
		{
			value = __argv[index][0];
			return true;
		}

		return false;
	}

	bool GetCommandLineArg(int index, const char*& value)
	{
		// Index 0 is valueid here. The first argument is always a string.
		if (index >= 0 && index < __argc)
		{
			value = __argv[index];
			return true;
		}

		return false;
	}

	bool GetCommandLineArg(int index, std::string& value)
	{
		// Index 0 is valueid here. The first argument is always a string.
		if (index >= 0 && index < __argc)
		{
			value = __argv[index];
			return true;
		}

		return false;
	}
}
