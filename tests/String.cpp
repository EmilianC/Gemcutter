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

		std::string str = " \t\n\n  TEMP string value... \t\n ";

		TrimStart(str);
		CHECK(str == "TEMP string value... \t\n ");

		TrimEnd(str);
		CHECK(str == "TEMP string value...");

		TrimStart(str);
		CHECK(str == "TEMP string value...");

		TrimEnd(str);
		CHECK(str == "TEMP string value...");

		str = {};
		TrimStart(str);
		CHECK(str == "");

		str = {};
		TrimEnd(str);
		CHECK(str == "");
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

	SECTION("Replace")
	{
		std::string str = "TEMP string is here!";

		SECTION("First")
		{
			ReplaceFirst(str, "", "test");
			CHECK(str == "TEMP string is here!");

			ReplaceFirst(str, "TEMP", "temp");
			CHECK(str == "temp string is here!");

			ReplaceFirst(str, "e", "o");
			CHECK(str == "tomp string is here!");

			ReplaceFirst(str, "tomp ", "");
			CHECK(str == "string is here!");
		}

		SECTION("Last")
		{
			ReplaceLast(str, "", "test");
			CHECK(str == "TEMP string is here!");

			ReplaceLast(str, "here", "ring");
			CHECK(str == "TEMP string is ring!");

			ReplaceLast(str, "ing", "oot");
			CHECK(str == "TEMP string is root!");

			ReplaceLast(str, "root", "");
			CHECK(str == "TEMP string is !");
		}

		SECTION("All")
		{
			ReplaceAll(str, "", "test");
			CHECK(str == "TEMP string is here!");

			ReplaceAll(str, " ", "_");
			CHECK(str == "TEMP_string_is_here!");

			ReplaceAll(str, "EMP", "");
			CHECK(str == "T_string_is_here!");

			ReplaceAll(str, "e", "ee");
			CHECK(str == "T_string_is_heeree!");
		}
	}
}
