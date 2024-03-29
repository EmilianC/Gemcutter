// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <string_view>

/*
 This file defines various functions and macros for logging.

 "Log_Output.txt" is appended with every message if it has been previously opened with OpenOutputLog().
 Console output can be toggled as well with CreateConsoleWindow() and DestroyConsoleWindow().
 DEBUG_* and ASSERT() macros allow you to log in debug builds.
*/

namespace gem
{
	enum class ConsoleColor : uint16_t
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

	void OpenOutputLog();
	void CloseOutputLog();

	void CreateConsoleWindow();
	void DestroyConsoleWindow();
	void FocusConsoleWindow();
	void SetConsoleColor(ConsoleColor color);
	void ResetConsoleColor();

	void Log(const char* format, ...);
	void Log(std::string_view message);
	void Error(const char* format, ...);
	void Error(std::string_view message);
	void Warning(const char* format, ...);
	void Warning(std::string_view message);
	void ErrorBox(const char* format, ...);
	void ErrorBox(std::string_view message);
	void WarningBox(const char* format, ...);
	void WarningBox(std::string_view message);

	void Assert(const char* exp, const char* format, ...);
}

#ifdef GEM_DEBUG
	#define DEBUG_LOG(format, ...) gem::Log((format), ##__VA_ARGS__)
	#define DEBUG_ERROR(format, ...) gem::Error((format), ##__VA_ARGS__)
	#define DEBUG_WARNING(format, ...) gem::Warning((format), ##__VA_ARGS__)
	#define DEBUG_ERROR_BOX(format, ...) gem::ErrorBox((format), ##__VA_ARGS__)
	#define DEBUG_WARNING_BOX(format, ...) gem::WarningBox((format), ##__VA_ARGS__)
	#define ASSERT(exp, format, ...) \
		do {                                                \
			if (!(exp)) [[unlikely]] {                      \
				gem::Assert(#exp, (format), ##__VA_ARGS__); \
				__debugbreak();                             \
			}                                               \
			__pragma(warning(suppress:4127))                \
		} while (false)
#else
	#define DEBUG_LOG(format, ...)
	#define DEBUG_ERROR(format, ...)
	#define DEBUG_WARNING(format, ...)
	#define DEBUG_ERROR_BOX(format, ...)
	#define DEBUG_WARNING_BOX(format, ...)
	#define ASSERT(exp, format, ...) __assume(exp)
#endif
