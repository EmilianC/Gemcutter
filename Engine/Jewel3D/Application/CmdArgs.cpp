// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "CmdArgs.h"

#include <stdlib.h>

namespace Jwl
{
	s32 GetArgc()
	{
		return __argc;
	}

	char** GetArgv()
	{
		return __argv;
	}

	bool ArgumentExists(const char* name)
	{
		for (s32 i = 1; i < __argc; i++)
		{
			// Search for the string.
			if (strcmp(__argv[i], name) == 0)
			{
				return true;
			}
		}

		return false;
	}

	s32 FindCommandLineArgument(const char* name)
	{
		for (s32 i = 1; i < __argc; i++)
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
		s32 index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = atoi(__argv[index]) == 1;
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(const char* name, f64& val)
	{
		s32 index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = atof(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(const char* name, f32& val)
	{
		s32 index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = static_cast<f32>(atof(__argv[index]));
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(const char* name, s32& val)
	{
		s32 index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = atoi(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(const char* name, u32& val)
	{
		s32 index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = atoi(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(const char* name, char& val)
	{
		s32 index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = __argv[index][0];
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(const char* name, std::string& val)
	{
		s32 index = FindCommandLineArgument(name) + 1;

		if (index > 0 && index < __argc)
		{
			val = __argv[index];
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(s32 index, bool& val)
	{
		if (index > 0 && index < __argc)
		{
			val = atoi(__argv[index]) == 1;
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(s32 index, f64& val)
	{
		if (index > 0 && index < __argc)
		{
			val = atof(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(s32 index, f32& val)
	{
		if (index > 0 && index < __argc)
		{
			val = static_cast<f32>(atof(__argv[index]));
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(s32 index, s32& val)
	{
		if (index > 0 && index < __argc)
		{
			val = atoi(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(s32 index, u32& val)
	{
		if (index > 0 && index < __argc)
		{
			val = atoi(__argv[index]);
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(s32 index, char& val)
	{
		if (index > 0 && index < __argc)
		{
			val = __argv[index][0];
			return true;
		}

		return false;
	}

	bool GetCommandLineArgument(s32 index, std::string& val)
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
