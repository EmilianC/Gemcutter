#include <catch/catch.hpp>
#include <gemcutter/Utilities/Meta.h>

using namespace gem;

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
}
