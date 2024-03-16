// Copyright (c) 2017 Emilian Cioca
#include "Logging.h"
#include "gemcutter/Utilities/String.h"

#include <fstream>
#include <iostream>
#include <loupe/loupe.h>
#include <stacktrace>
#include <Windows.h>

namespace
{
	std::ofstream logOutput;
	HANDLE stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	void PushMessage(std::string_view header, std::string_view message)
	{
		if (logOutput.is_open())
		{
			logOutput << header << message << std::endl;
		}

		if (stdOutputHandle != INVALID_HANDLE_VALUE)
		{
			std::cout << header << message << std::endl;
		}

#if defined(_MSC_VER) && defined(GEM_DEBUG)
		OutputDebugString(header.data());
		OutputDebugString(message.data());
		OutputDebugString("\n");
#endif
	}

	void PushMessage(std::string_view header, std::string_view message, gem::ConsoleColor color)
	{
		gem::SetConsoleColor(color);
		PushMessage(header, message);
		gem::ResetConsoleColor();
	}
}

namespace gem
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

	void FocusConsoleWindow()
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

		PushMessage("Log:     ", message);
	}

	void Log(std::string_view message)
	{
		PushMessage("Log:     ", message);
	}

	void Error(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		PushMessage("ERROR:   ", message, ConsoleColor::Red);
	}

	void Error(std::string_view message)
	{
		PushMessage("ERROR:   ", message, ConsoleColor::Red);
	}

	void Warning(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		PushMessage("WARNING: ", message, ConsoleColor::Yellow);
	}

	void Warning(std::string_view message)
	{
		PushMessage("WARNING: ", message, ConsoleColor::Yellow);
	}

	void ErrorBox(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		PushMessage("ERROR:   ", message, ConsoleColor::Red);
		MessageBox(HWND_DESKTOP, message.c_str(), "Error", MB_ICONERROR);
	}

	void ErrorBox(std::string_view message)
	{
		PushMessage("ERROR:   ", message, ConsoleColor::Red);
		MessageBox(HWND_DESKTOP, message.data(), "Error", MB_ICONERROR);
	}

	void WarningBox(const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		PushMessage("WARNING: ", message, ConsoleColor::Yellow);
		MessageBox(HWND_DESKTOP, message.c_str(), "Warning", MB_ICONWARNING);
	}

	void WarningBox(std::string_view message)
	{
		PushMessage("WARNING: ", message, ConsoleColor::Yellow);
		MessageBox(HWND_DESKTOP, message.data(), "Warning", MB_ICONWARNING);
	}

	void Assert(const char* exp, const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		auto message = FormatString(format, argptr);
		va_end(argptr);

		message += "\n-------\n";
		message += std::to_string(std::stacktrace::current(1));

		auto header = FormatString("ASSERT:  ( %s )\n", exp);
		PushMessage(header, message, ConsoleColor::Pink);
	}
}

REFLECT(gem::ConsoleColor)
	ENUM_VALUES {
		REF_VALUE(DarkBlue)
		REF_VALUE(DarkGreen)
		REF_VALUE(DarkTeal)
		REF_VALUE(DarkRed)
		REF_VALUE(DarkPink)
		REF_VALUE(DarkYellow)
		REF_VALUE(Gray)
		REF_VALUE(DarkGray)
		REF_VALUE(Blue)
		REF_VALUE(Green)
		REF_VALUE(Teal)
		REF_VALUE(Red)
		REF_VALUE(Pink)
		REF_VALUE(Yellow)
		REF_VALUE(White)
	}
REF_END;
