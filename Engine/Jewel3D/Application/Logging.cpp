// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Logging.h"
#include "Jewel3D/Utilities/String.h"

#include <iostream>
#include <fstream>
#include <Windows.h>

namespace
{
	static std::ofstream logOutput{ "Log_Output.txt", std::ofstream::app };
	static HANDLE stdOutputHandle = NULL;
	static bool consoleOutput = false;

	enum class LogColor
	{
		DarkBlue = 1,
		DarkGreen,
		DarkTeal,
		DarkRed,
		DarkPink,
		DarkYellow,
		Gray,
		DarkGray,
		Blue,
		Green,
		Teal,
		Red,
		Pink,
		Yellow,
		White
	};

	static void SetColor(LogColor color)
	{
		SetConsoleTextAttribute(stdOutputHandle, (WORD)color);
	}

	static void ResetToDefaultColor()
	{
		SetConsoleTextAttribute(stdOutputHandle, (WORD)LogColor::Gray);
	}
}

namespace Jwl
{
	void CreateConsoleWindow()
	{
		if (consoleOutput)
		{
			return;
		}

		AllocConsole();
		SetConsoleTitle("Output Log");
		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
		stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		consoleOutput = true;
	}

	void DestroyConsoleWindow()
	{
		if (!consoleOutput)
		{
			return;
		}

		fclose(stdout);
		fclose(stdin);
		FreeConsole();
		stdOutputHandle = NULL;

		consoleOutput = false;
	}

	void MoveConsoleWindowToForeground()
	{
		if (!consoleOutput)
		{
			return;
		}

		SetForegroundWindow(GetConsoleWindow());
	}

	void Log(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		// std::endl forces a flush to file.
		logOutput << "Log:\t\t" << message << std::endl;

		if (consoleOutput)
		{
			std::cout << "Log:     " << message << std::endl;
		}
	}

	void Log(const std::string& message)
	{
		logOutput << "Log:\t\t" << message << std::endl;

		if (consoleOutput)
		{
			std::cout << "Log:     " << message << std::endl;
		}
	}

	void Error(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		logOutput << "ERROR:\t\t" << message << std::endl;

		if (consoleOutput)
		{
			SetColor(LogColor::Red);
			std::cout << "ERROR:   " << message << std::endl;
			ResetToDefaultColor();
		}
	}

	void Error(const std::string& message)
	{
		logOutput << "ERROR:\t\t" << message << std::endl;

		if (consoleOutput)
		{
			SetColor(LogColor::Red);
			std::cout << "ERROR:   " << message << std::endl;
			ResetToDefaultColor();
		}
	}

	void Warning(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		logOutput << "WARNING:\t" << message << std::endl;

		if (consoleOutput)
		{
			SetColor(LogColor::Yellow);
			std::cout << "WARNING: " << message << std::endl;
			ResetToDefaultColor();
		}
	}

	void Warning(const std::string& message)
	{
		logOutput << "WARNING:\t" << message << std::endl;

		if (consoleOutput)
		{
			SetColor(LogColor::Yellow);
			std::cout << "WARNING: " << message << std::endl;
			ResetToDefaultColor();
		}
	}

	void ErrorBox(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		logOutput << "ERROR:\t\t" << message << std::endl;

		if (consoleOutput)
		{
			SetColor(LogColor::Red);
			std::cout << "ERROR:   " << message << std::endl;
			ResetToDefaultColor();
		}

		MessageBox(HWND_DESKTOP, message.c_str(), "Error", MB_ICONERROR);
	}
	void ErrorBox(const std::string& message)
	{
		logOutput << "ERROR:\t\t" << message << std::endl;

		if (consoleOutput)
		{
			SetColor(LogColor::Red);
			std::cout << "ERROR:   " << message << std::endl;
			ResetToDefaultColor();
		}

		MessageBox(HWND_DESKTOP, message.c_str(), "Error", MB_ICONERROR);
	}

	void WarningBox(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		logOutput << "WARNING:\t" << message << std::endl;

		if (consoleOutput)
		{
			SetColor(LogColor::Yellow);
			std::cout << "WARNING: " << message << std::endl;
			ResetToDefaultColor();
		}

		MessageBox(HWND_DESKTOP, message.c_str(), "Warning", MB_ICONWARNING);
	}

	void WarningBox(const std::string& message)
	{
		logOutput << "WARNING:\t" << message << std::endl;

		if (consoleOutput)
		{
			SetColor(LogColor::Yellow);
			std::cout << "WARNING: " << message << std::endl;
			ResetToDefaultColor();
		}

		MessageBox(HWND_DESKTOP, message.c_str(), "Warning", MB_ICONWARNING);
	}

	void Assert(const char* exp, const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		logOutput << "ASSERT:\t\t( " << exp << " )\n" << message << std::endl;

		if (consoleOutput)
		{
			SetColor(LogColor::Pink);
			std::cout << "ASSERT:  ( " << exp << " )\n" << message << std::endl;
			ResetToDefaultColor();
		}
	}
}
