#include <catch/catch.hpp>
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

	SECTION("GetTypeName")
	{
		CHECK(meta::GetTypeName<void>() == "void");
		CHECK(meta::GetTypeName<void*>() == "void*");
		CHECK(meta::GetTypeName<int>() == "int");
		CHECK(meta::GetTypeName<const int>() == "int");
		CHECK(meta::GetTypeName<std::optional<int>>() == "std::optional<int>");

		CHECK(meta::GetTypeName<types::MyClass>() == "types::MyClass");
		CHECK(meta::GetTypeName<types::MyStruct>() == "types::MyStruct");
		CHECK(meta::GetTypeName<types::StrongEnum>() == "types::StrongEnum");
		CHECK(meta::GetTypeName<types::WeakEnum>() == "types::WeakEnum");

		CHECK(meta::GetTypeName<const types::MyClass>() == "types::MyClass");
		CHECK(meta::GetTypeName<const types::MyStruct>() == "types::MyStruct");
		CHECK(meta::GetTypeName<const types::StrongEnum>() == "types::StrongEnum");
		CHECK(meta::GetTypeName<const types::WeakEnum>() == "types::WeakEnum");
	}
}
