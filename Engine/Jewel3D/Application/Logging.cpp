// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Logging.h"
#include "Jewel3D/Utilities/String.h"

#include <iostream>
#include <fstream>
#include <Windows.h>

namespace
{
	static std::ofstream logOutput;
	static HANDLE stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

namespace Jwl
{
	void OpenOutputLog()
	{
		if (!logOutput.is_open())
		{
			logOutput.open("Log_Output.txt", std::ofstream::app);
		}
	}

	void CloseOutputLog()
	{
		if (logOutput.is_open())
		{
			logOutput.close();
		}
	}

	void CreateConsoleWindow()
	{
		if (GetConsoleWindow() != NULL)
		{
			return;
		}

		AllocConsole();
		SetConsoleTitle("Output Log");
		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
		stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	void DestroyConsoleWindow()
	{
		if (GetConsoleWindow() == NULL)
		{
			return;
		}

		fclose(stdout);
		fclose(stdin);
		FreeConsole();
		
		// There might still be an output stream so we try to obtain a new handle just in case.
		stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	void MoveConsoleWindowToForeground()
	{
		SetForegroundWindow(GetConsoleWindow());
	}

	void SetConsoleColor(ConsoleColor color)
	{
		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			SetConsoleTextAttribute(stdOutputHandle, static_cast<WORD>(color));
		}
	}

	void ResetConsoleColor()
	{
		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			SetConsoleTextAttribute(stdOutputHandle, static_cast<WORD>(ConsoleColor::Gray));
		}
	}

	void Log(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		// std::endl forces a flush to file.
		if (logOutput.is_open())
		{
			logOutput << "Log:\t\t" << message << std::endl;
		}

		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			std::cout << "Log:     " << message << std::endl;
		}
	}

	void Log(const std::string& message)
	{
		if (logOutput.is_open())
		{
			logOutput << "Log:\t\t" << message << std::endl;
		}

		if (stdOutputHandle != INVALID_HANDLE_VALUE)
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

		if (logOutput.is_open())
		{
			logOutput << "ERROR:\t\t" << message << std::endl;
		}

		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			SetConsoleColor(ConsoleColor::Red);
			std::cout << "ERROR:   " << message << std::endl;
			ResetConsoleColor();
		}
	}

	void Error(const std::string& message)
	{
		if (logOutput.is_open())
		{
			logOutput << "ERROR:\t\t" << message << std::endl;
		}

		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			SetConsoleColor(ConsoleColor::Red);
			std::cout << "ERROR:   " << message << std::endl;
			ResetConsoleColor();
		}
	}

	void Warning(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		if (logOutput.is_open())
		{
			logOutput << "WARNING:\t" << message << std::endl;
		}

		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			SetConsoleColor(ConsoleColor::Yellow);
			std::cout << "WARNING: " << message << std::endl;
			ResetConsoleColor();
		}
	}

	void Warning(const std::string& message)
	{
		if (logOutput.is_open())
		{
			logOutput << "WARNING:\t" << message << std::endl;
		}

		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			SetConsoleColor(ConsoleColor::Yellow);
			std::cout << "WARNING: " << message << std::endl;
			ResetConsoleColor();
		}
	}

	void ErrorBox(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		if (logOutput.is_open())
		{
			logOutput << "ERROR:\t\t" << message << std::endl;
		}

		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			SetConsoleColor(ConsoleColor::Red);
			std::cout << "ERROR:   " << message << std::endl;
			ResetConsoleColor();
		}

		MessageBox(HWND_DESKTOP, message.c_str(), "Error", MB_ICONERROR);
	}
	void ErrorBox(const std::string& message)
	{
		if (logOutput.is_open())
		{
			logOutput << "ERROR:\t\t" << message << std::endl;
		}

		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			SetConsoleColor(ConsoleColor::Red);
			std::cout << "ERROR:   " << message << std::endl;
			ResetConsoleColor();
		}

		MessageBox(HWND_DESKTOP, message.c_str(), "Error", MB_ICONERROR);
	}

	void WarningBox(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		if (logOutput.is_open())
		{
			logOutput << "WARNING:\t" << message << std::endl;
		}

		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			SetConsoleColor(ConsoleColor::Yellow);
			std::cout << "WARNING: " << message << std::endl;
			ResetConsoleColor();
		}

		MessageBox(HWND_DESKTOP, message.c_str(), "Warning", MB_ICONWARNING);
	}

	void WarningBox(const std::string& message)
	{
		if (logOutput.is_open())
		{
			logOutput << "WARNING:\t" << message << std::endl;
		}

		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			SetConsoleColor(ConsoleColor::Yellow);
			std::cout << "WARNING: " << message << std::endl;
			ResetConsoleColor();
		}

		MessageBox(HWND_DESKTOP, message.c_str(), "Warning", MB_ICONWARNING);
	}

	void Assert(const char* exp, const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		if (logOutput.is_open())
		{
			logOutput << "ASSERT:\t\t( " << exp << " )\n" << message << std::endl;
		}

		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			SetConsoleColor(ConsoleColor::Pink);
			std::cout << "ASSERT:  ( " << exp << " )\n" << message << std::endl;
			ResetConsoleColor();
		}
	}
}
