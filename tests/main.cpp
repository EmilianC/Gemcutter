#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_COLOUR_WINDOWS
#include <catch/catch.hpp>
#include <gemcutter/Application/CmdArgs.h>
#include <gemcutter/Application/Logging.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	using namespace Jwl;

	CreateConsoleWindow();

	int result = Catch::Session().run(GetArgc(), GetArgv());

	FocusConsoleWindow();
	if (result != 0)
	{
		system("pause");
	}
	else
	{
		Sleep(800);
	}

	return result;
}
