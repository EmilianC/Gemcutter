#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>
#include <chrono>
#include <print>
#include <thread>

#include <gemcutter/Application/Reflection.h>

int main(int argc, char** argv)
{
	gem::InitializeReflectionTables();

	const int result = Catch::Session().run(argc, argv);
	if (result != 0)
	{
		std::println("Press any key to continue...");
		std::cin.get();
	}
	else
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return result;
}
