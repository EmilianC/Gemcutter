#include <catch/catch.hpp>
#include <gemcutter/Utilities/EnumFlags.h>

using namespace gem;

enum class Values : uint16_t
{
	One   = 0b0001,
	Two   = 0b0010,
	Three = 0b0100,
	Four  = 0b1000
};

static_assert(std::is_trivially_copyable_v<EnumFlags<Values>>);
static_assert(std::is_same_v<EnumFlags<Values>::PrimitiveType, uint16_t>);

TEST_CASE("EnumFlags")
{
	auto flags = EnumFlags<Values>();

	CHECK(!flags.Has(Values::One));
	CHECK(!flags.Has(Values::Two));
	CHECK(!flags.Has(Values::Three));
	CHECK(!flags.Has(Values::Four));
	CHECK(flags.Value() == 0);
	CHECK(flags == 0b0000);

	SECTION("OR-ing Flags")
	{
		flags |= Values::One;
		CHECK(flags.Has(Values::One));
		CHECK(!flags.Has(Values::Two));
		CHECK(!flags.Has(Values::Three));
		CHECK(!flags.Has(Values::Four));
		CHECK(flags.Value() == 0b0001);
		CHECK(flags == 0b0001);

		flags |= EnumFlags{Values::Two};
		CHECK(flags.Has(Values::One));
		CHECK(flags.Has(Values::Two));
		CHECK(!flags.Has(Values::Three));
		CHECK(!flags.Has(Values::Four));
		CHECK(flags.Value() == 0b0011);
		CHECK(flags == 0b0011);

		flags |= 0b0100;
		CHECK(flags.Has(Values::One));
		CHECK(flags.Has(Values::Two));
		CHECK(flags.Has(Values::Three));
		CHECK(!flags.Has(Values::Four));
		CHECK(flags.Value() == 0b0111);
		CHECK(flags == 0b0111);

		flags |= Values::Four;
		CHECK(flags.Has(0b0001));
		CHECK(flags.Has(0b0010));
		CHECK(flags.Has(0b0100));
		CHECK(flags.Has(0b1000));
		CHECK(flags.Value() == 0b1111);
		CHECK(flags == 0b1111);
	}

	SECTION("AND-ing Flags")
	{
		flags = 0b1100;
		CHECK(!flags.Has(Values::One));
		CHECK(!flags.Has(Values::Two));
		CHECK(flags.Has(Values::Three));
		CHECK(flags.Has(Values::Four));
		CHECK(flags.Value() == 0b1100);
		CHECK(flags == 0b1100);

		SECTION("Test 1")
		{
			flags &= Values::Three;
			CHECK(!flags.Has(Values::One));
			CHECK(!flags.Has(Values::Two));
			CHECK(flags.Has(Values::Three));
			CHECK(!flags.Has(Values::Four));
			CHECK(flags.Value() == 0b0100);
			CHECK(flags == 0b0100);
		}

		SECTION("Test 2")
		{
			flags &= 0b1111;
			CHECK(!flags.Has(Values::One));
			CHECK(!flags.Has(Values::Two));
			CHECK(flags.Has(Values::Three));
			CHECK(flags.Has(Values::Four));
			CHECK(flags.Value() == 0b1100);
			CHECK(flags == 0b1100);
		}

		SECTION("Test 3")
		{
			flags = flags & 0b1011;
			CHECK(!flags.Has(Values::One));
			CHECK(!flags.Has(Values::Two));
			CHECK(!flags.Has(Values::Three));
			CHECK(flags.Has(Values::Four));
			CHECK(flags.Value() == 0b1000);
			CHECK(flags == 0b1000);
		}
	}

	flags.Clear();
	CHECK(!flags.Has(Values::One));
	CHECK(!flags.Has(Values::Two));
	CHECK(!flags.Has(Values::Three));
	CHECK(!flags.Has(Values::Four));
	CHECK(flags.Value() == 0b0000);
	CHECK(flags == 0b0000);
}
