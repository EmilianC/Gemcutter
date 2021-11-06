#include <catch/catch.hpp>
#include <gemcutter/Utilities/Meta.h>

using namespace gem;

TEST_CASE("Metaprogramming")
{
	SECTION("all_of")
	{
		CHECK(Meta::all_of_v<>);

		CHECK(Meta::all_of_v<true>);
		CHECK(Meta::all_of_v<true, true>);
		CHECK(Meta::all_of_v<true, true, true>);

		CHECK(!Meta::all_of_v<false>);
		CHECK(!Meta::all_of_v<false, false>);
		CHECK(!Meta::all_of_v<false, false, false>);

		CHECK(!Meta::all_of_v<false>);
		CHECK(!Meta::all_of_v<false, true>);
		CHECK(!Meta::all_of_v<true, false>);
		CHECK(!Meta::all_of_v<true, true, false>);
	}

	SECTION("any_of")
	{
		CHECK(!Meta::any_of_v<>);

		CHECK(Meta::any_of_v<true>);
		CHECK(Meta::any_of_v<true, true>);
		CHECK(Meta::any_of_v<true, true, true>);

		CHECK(!Meta::any_of_v<false>);
		CHECK(!Meta::any_of_v<false, false>);
		CHECK(!Meta::any_of_v<false, false, false>);

		CHECK(!Meta::any_of_v<false>);
		CHECK(Meta::any_of_v<false, true>);
		CHECK(Meta::any_of_v<true, false>);
		CHECK(Meta::any_of_v<true, true, false>);
	}

	SECTION("none_of")
	{
		CHECK(Meta::none_of_v<>);

		CHECK(!Meta::none_of_v<true>);
		CHECK(!Meta::none_of_v<true, true>);
		CHECK(!Meta::none_of_v<true, true, true>);

		CHECK(Meta::none_of_v<false>);
		CHECK(Meta::none_of_v<false, false>);
		CHECK(Meta::none_of_v<false, false, false>);

		CHECK(Meta::none_of_v<false>);
		CHECK(!Meta::none_of_v<false, true>);
		CHECK(!Meta::none_of_v<true, false>);
		CHECK(!Meta::none_of_v<true, true, false>);
	}
}
