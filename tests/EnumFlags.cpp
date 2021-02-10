#include <catch/catch.hpp>
#include <gemcutter/Utilities/EnumFlags.h>

using namespace gem;

enum class Values
{
	One   = 0b0001,
	Two   = 0b0010,
	Three = 0b0100,
	Four  = 0b1000,
};

static_assert(std::is_trivially_copyable_v<EnumFlags<Values>>);

TEST_CASE("EnumFlags")
{
	auto flags = EnumFlags<Values>();

	CHECK(!flags.Has(Values::One));
	CHECK(!flags.Has(Values::Two));
	CHECK(!flags.Has(Values::Three));
	CHECK(!flags.Has(Values::Four));

	SECTION("OR-ing Flags")
	{
		flags |= Values::One;
		CHECK(flags.Has(Values::One));
		CHECK(!flags.Has(Values::Two));
		CHECK(!flags.Has(Values::Three));
		CHECK(!flags.Has(Values::Four));

		flags |= Values::Two;
		CHECK(flags.Has(Values::One));
		CHECK(flags.Has(Values::Two));
		CHECK(!flags.Has(Values::Three));
		CHECK(!flags.Has(Values::Four));

		flags |= 0b0100;
		CHECK(flags.Has(Values::One));
		CHECK(flags.Has(Values::Two));
		CHECK(flags.Has(Values::Three));
		CHECK(!flags.Has(Values::Four));

		flags |= Values::Four;
		CHECK(flags.Has(0b0001));
		CHECK(flags.Has(0b0010));
		CHECK(flags.Has(0b0100));
		CHECK(flags.Has(0b1000));
	}

	SECTION("AND-ing Flags")
	{
		flags = 0b1100;
		CHECK(!flags.Has(Values::One));
		CHECK(!flags.Has(Values::Two));
		CHECK(flags.Has(Values::Three));
		CHECK(flags.Has(Values::Four));

		SECTION("Test 1")
		{
			flags &= Values::Three;
			CHECK(!flags.Has(Values::One));
			CHECK(!flags.Has(Values::Two));
			CHECK(flags.Has(Values::Three));
			CHECK(!flags.Has(Values::Four));
		}

		SECTION("Test 2")
		{
			flags &= 0b1111;
			CHECK(!flags.Has(Values::One));
			CHECK(!flags.Has(Values::Two));
			CHECK(flags.Has(Values::Three));
			CHECK(flags.Has(Values::Four));
		}

		SECTION("Test 3")
		{
			flags = flags & 0b1011;
			CHECK(!flags.Has(Values::One));
			CHECK(!flags.Has(Values::Two));
			CHECK(!flags.Has(Values::Three));
			CHECK(flags.Has(Values::Four));
		}
	}

	flags.Clear();
	CHECK(!flags.Has(Values::One));
	CHECK(!flags.Has(Values::Two));
	CHECK(!flags.Has(Values::Three));
	CHECK(!flags.Has(Values::Four));
}
