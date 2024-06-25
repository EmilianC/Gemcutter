#include <catch/catch.hpp>
#include <gemcutter/Application/Reflection.h>
#include <gemcutter/Utilities/Meta.h>

using namespace gem;

namespace types
{
	class MyClass {};
	struct MyStruct {};
	enum class StrongEnum {};
	enum WeakEnum {};
}

TEST_CASE("Metaprogramming")
{
	SECTION("all_of")
	{
		CHECK(meta::all_of_v<>);

		CHECK(meta::all_of_v<true>);
		CHECK(meta::all_of_v<true, true>);
		CHECK(meta::all_of_v<true, true, true>);

		CHECK(!meta::all_of_v<false>);
		CHECK(!meta::all_of_v<false, false>);
		CHECK(!meta::all_of_v<false, false, false>);

		CHECK(!meta::all_of_v<false>);
		CHECK(!meta::all_of_v<false, true>);
		CHECK(!meta::all_of_v<true, false>);
		CHECK(!meta::all_of_v<true, true, false>);
	}

	SECTION("any_of")
	{
		CHECK(!meta::any_of_v<>);

		CHECK(meta::any_of_v<true>);
		CHECK(meta::any_of_v<true, true>);
		CHECK(meta::any_of_v<true, true, true>);

		CHECK(!meta::any_of_v<false>);
		CHECK(!meta::any_of_v<false, false>);
		CHECK(!meta::any_of_v<false, false, false>);

		CHECK(!meta::any_of_v<false>);
		CHECK(meta::any_of_v<false, true>);
		CHECK(meta::any_of_v<true, false>);
		CHECK(meta::any_of_v<true, true, false>);
	}

	SECTION("none_of")
	{
		CHECK(meta::none_of_v<>);

		CHECK(!meta::none_of_v<true>);
		CHECK(!meta::none_of_v<true, true>);
		CHECK(!meta::none_of_v<true, true, true>);

		CHECK(meta::none_of_v<false>);
		CHECK(meta::none_of_v<false, false>);
		CHECK(meta::none_of_v<false, false, false>);

		CHECK(meta::none_of_v<false>);
		CHECK(!meta::none_of_v<false, true>);
		CHECK(!meta::none_of_v<true, false>);
		CHECK(!meta::none_of_v<true, true, false>);
	}

	SECTION("is_any_of")
	{
		CHECK(!meta::is_any_of_v<bool>);

		CHECK(meta::is_any_of_v<bool, bool, int, float>);
		CHECK(meta::is_any_of_v<bool, float, int, bool>);

		CHECK(!meta::is_any_of_v<bool, int, float>);
		CHECK(!meta::is_any_of_v<bool, double, double>);
		CHECK(!meta::is_any_of_v<bool, const bool>);
	}

	SECTION("is_none_of")
	{
		CHECK(meta::is_none_of_v<bool>);

		CHECK(meta::is_none_of_v<bool, int, float, double>);
		CHECK(meta::is_none_of_v<bool, char, float>);
		CHECK(meta::is_none_of_v<bool, char, char>);
		CHECK(meta::is_none_of_v<bool, const bool>);

		CHECK(!meta::is_none_of_v<bool, bool, int, float>);
		CHECK(!meta::is_none_of_v<bool, float, int, bool>);
	}

	SECTION("GetTypeName")
	{
		CHECK(GetTypeName<void>() == "void");
		CHECK(GetTypeName<void*>() == "void*");
		CHECK(GetTypeName<int>() == "int");
		CHECK(GetTypeName<const int>() == "int");
		CHECK(GetTypeName<std::optional<int>>() == "std::optional<int>");

		CHECK(GetTypeName<types::MyClass>() == "types::MyClass");
		CHECK(GetTypeName<types::MyStruct>() == "types::MyStruct");
		CHECK(GetTypeName<types::StrongEnum>() == "types::StrongEnum");
		CHECK(GetTypeName<types::WeakEnum>() == "types::WeakEnum");

		CHECK(GetTypeName<const types::MyClass>() == "types::MyClass");
		CHECK(GetTypeName<const types::MyStruct>() == "types::MyStruct");
		CHECK(GetTypeName<const types::StrongEnum>() == "types::StrongEnum");
		CHECK(GetTypeName<const types::WeakEnum>() == "types::WeakEnum");
	}
}
