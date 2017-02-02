#include <catch.hpp>
#include <Jewel3D/Math/Math.h>

using namespace Jwl;

TEST_CASE("Math")
{
	SECTION("Abs / Min / Max")
	{
		CHECK(Abs(1) == 1);
		CHECK(Abs(-2.5f) == 2.5f);

		CHECK(Min(2.5f, 2.0f) == 2.0f);
		CHECK(Min(1, 2, 3, 4, 5, 6) == 1);

		CHECK(Max(2.5f, 2.0f) == 2.5f);
		CHECK(Max(1, 3, 5, 6, 2, 4) == 6);

		CHECK(Clamp(10, 5, 15) == 10);
		CHECK(Clamp(0, 5, 15) == 5);
		CHECK(Clamp(20, 5, 15) == 15);
	}
}
