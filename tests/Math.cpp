#include <catch/catch.hpp>
#include <gemcutter/Math/Math.h>

using namespace gem;

TEST_CASE("Math")
{
	SECTION("Utility Functions")
	{
		CHECK(Abs(0) == 0);
		CHECK(Abs(1) == 1);
		CHECK(Abs(-2.5f) == 2.5f);

		CHECK(Equals(0.0f, FLT_EPSILON));
		CHECK(Equals(0.0, DBL_EPSILON));
		CHECK(Equals(0.0f, 0.0f));
		CHECK(Equals(1.0f, 1.0f));
		CHECK(Equals(-1.0f, -1.0f));
		CHECK(Equals(1.0f, 1.0f + FLT_EPSILON));
		CHECK(Equals(1.0, 1.0 + DBL_EPSILON));
		CHECK_FALSE(Equals(0.0f - FLT_EPSILON, 0.0f + FLT_EPSILON));
		CHECK_FALSE(Equals(0.0 - DBL_EPSILON, 0.0 + DBL_EPSILON));
		CHECK_FALSE(Equals(-1.0f, 1.0f));
		CHECK_FALSE(Equals(-1.0, 1.0));
		CHECK_FALSE(Equals(FLT_EPSILON, -FLT_EPSILON));
		CHECK_FALSE(Equals(DBL_EPSILON, -DBL_EPSILON));

		CHECK(Min(0.0f, 0.0f) == 0.0f);
		CHECK(Min(2.5f, 2.0f) == 2.0f);
		CHECK(Min(1, 2, 3, 4, 5, 6) == 1);

		CHECK(Max(0.0f, 0.0f) == 0.0f);
		CHECK(Max(2.5f, 2.0f) == 2.5f);
		CHECK(Max(1, 3, 5, 6, 2, 4) == 6);

		CHECK(Clamp(0, 0, 0) == 0);
		CHECK(Clamp(10, 5, 15) == 10);
		CHECK(Clamp(0, 5, 15) == 5);
		CHECK(Clamp(20, 5, 15) == 15);

		CHECK(Equals(PercentInRange(5.0f, 0.0f, 10.0f), 0.5f));
		CHECK(Equals(PercentInRange(-1.0f, -2.0f, 2.0f), 0.25f));
		CHECK(Equals(PercentInRange(3.0f, 0.0f, 1.0f), 3.0f));
		CHECK(Equals(PercentInRange(0.0f, 1.0f, 1.0f), 0.0f));

		CHECK(PowerOfTwoCeil(0u) == 1u);
		CHECK(PowerOfTwoCeil(1u) == 1u);
		CHECK(PowerOfTwoCeil(2u) == 2u);
		CHECK(PowerOfTwoCeil(3u) == 4u);
		CHECK(PowerOfTwoCeil(10u) == 16u);
		CHECK(PowerOfTwoCeil(1025u) == 2048u);
		CHECK(PowerOfTwoCeil(2048u) == 2048u);

		CHECK(PowerOfTwoFloor(0u) == 0u);
		CHECK(PowerOfTwoFloor(1u) == 1u);
		CHECK(PowerOfTwoFloor(2u) == 2u);
		CHECK(PowerOfTwoFloor(3u) == 2u);
		CHECK(PowerOfTwoFloor(10u) == 8u);
		CHECK(PowerOfTwoFloor(1025u) == 1024u);
		CHECK(PowerOfTwoFloor(2048u) == 2048u);
	}
}
