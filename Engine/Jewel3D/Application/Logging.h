// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <string_view>

/*
 This file defines various functions and macros for logging.

 "Log_Output.txt" is appended with every message if it has been previously opened with OpenOutputLog().
 Console output can be toggled as well with CreateConsoleWindow() and DestroyConsoleWindow().
 DEBUG_* macros allow you to log only when in debug mode.

 To force macros off, define JWL_DISABLE_DEBUG_MESSAGES.
*/

namespace Jwl
{
	enum class ConsoleColor
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

#if defined(_DEBUG) && !defined(JWL_DISABLE_DEBUG_MESSAGES)
	#define DEBUG_LOG(format, ...) Jwl::Log((format), ##__VA_ARGS__)
	#define DEBUG_ERROR(format, ...) Jwl::Error((format), ##__VA_ARGS__)
	#define DEBUG_WARNING(format, ...) Jwl::Warning((format), ##__VA_ARGS__)
	#define DEBUG_ERROR_BOX(format, ...) Jwl::ErrorBox((format), ##__VA_ARGS__)
	#define DEBUG_WARNING_BOX(format, ...) Jwl::WarningBox((format), ##__VA_ARGS__)
	#define ASSERT(exp, format, ...) \
		do {                                                \
			if (!(exp)) {                                   \
				Jwl::Assert(#exp, (format), ##__VA_ARGS__); \
				__asm { int 3 }                             \
			}                                               \
			__pragma(warning(suppress:4127))                \
		} while (false)
#else
	#define DEBUG_LOG __noop
	#define DEBUG_ERROR __noop
	#define DEBUG_WARNING __noop
	#define DEBUG_ERROR_BOX __noop
	#define DEBUG_WARNING_BOX __noop
	#define ASSERT __noop
#endif
