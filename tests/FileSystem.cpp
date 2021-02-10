#include <catch/catch.hpp>
#include <gemcutter/Application/FileSystem.h>

using namespace gem;

TEST_CASE("FileSystem")
{
	SECTION("Directories and Filenames")
	{
		CHECK(DirectoryExists("C:/"));

		CHECK(IsPathAbsolute("C:/"));

		CHECK_FALSE(IsPathRelative("C:/"));

		CHECK(IsPathRelative("./Folder1/Folder2/"));

		CHECK(IsPathRelative("/Folder1/Folder2/"));

		CHECK(ExtractDriveLetter("C:/user/test.txt") == 'C');

		CHECK(ExtractPath("C:/user/test.txt") == "C:/user/");

		CHECK(ExtractPath("./user/test.txt") == "./user/");

		CHECK(ExtractFile("C:/user/test2.txt") == "test2.txt");

		CHECK(ExtractFilename("C:/user/test3.txt") == "test3");

		CHECK(ExtractFileExtension("C:/user/test4.txt") == ".txt");

		CHECK(ExtractFileExtension("C:/user/test5.txt.zip") == ".zip");
	}
}
