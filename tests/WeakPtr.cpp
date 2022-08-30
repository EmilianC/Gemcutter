#include <catch/catch.hpp>
#include <gemcutter/Utilities/StdExt.h>

using namespace gem;

TEST_CASE("WeakPtr")
{
	std::shared_ptr<int> shared = std::make_shared<int>(1);
	std::weak_ptr<int> weak = shared;
	std::weak_ptr<int> weakNull;
	std::weak_ptr<int> weakExpired = std::make_shared<int>(1);

	SECTION("Null Checks")
	{
		CHECK(!IsPtrNull(weak));
		CHECK(IsPtrNull(weakNull));
		CHECK(!IsPtrNull(weakExpired));
	}
}
