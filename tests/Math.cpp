#include <catch.hpp>
#include <Jewel3D/Math/Math.h>

using namespace Jwl;

TEST_CASE("Math")
{
	SECTION("Abs / Equals / Min / Max / Clamp")
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
	}
}
