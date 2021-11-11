#include <catch/catch.hpp>
#include <gemcutter/Utilities/String.h>

using namespace gem;

TEST_CASE("String")
{
	std::string input =
		"// Comment1\n"
		"int  temp  =\t\t 10  ;\n"
		"/* Comment 2 */\n";

	SECTION("Remove Comments")
	{
		RemoveComments(input);

		CHECK(input == "int  temp  =\t\t 10  ;\n\n");
	}

	SECTION("Remove Whitespace")
	{
		RemoveWhitespace(input);

		CHECK(input == "//Comment1"
			"inttemp=10;"
			"/*Comment2*/");
	}

	SECTION("Remove Redundant Whitespace")
	{
		RemoveRedundantWhitespace(input);

		CHECK(input ==
			"// Comment1\n"
			"int temp = 10 ;\n"
			"/* Comment 2 */\n");
	}

	SECTION("Sanitize Code")
	{
		RemoveComments(input);
		RemoveRedundantWhitespace(input);

		CHECK(input == "int temp = 10 ;\n");
	}

	SECTION("ToLowercase")
	{
		std::string str = "TEMP !@#$%^&*()-= temp";
		ToLowercase(str);

		CHECK(str == "temp !@#$%^&*()-= temp");
	}
}
