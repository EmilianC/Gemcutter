#include <catch.hpp>

#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Reflection/Reflection.h>
#include <Jewel3D/Utilities/Meta.h>
#include <vector>
#include <string>

using namespace Jwl;

namespace Reflection
{
	struct TestTag : MetaTagBase {};
}

class Base
{
	REFLECT_PRIVATE;

	class PrivateClass
	{
	};

	enum class PrivateEnum
	{
	};
};

REFLECT_BASIC(Base)
REFLECT_BASIC(Base::PrivateClass)
REFLECT_BASIC(Base::PrivateEnum)

class ClassTest : Base
{
	REFLECT_PRIVATE;
public:
	ClassTest() = default;

	int Int = 123;

	const float* floatPtr = nullptr;

	bool setterWasCalled = false;
	void IntSetter(int val)
	{
		setterWasCalled = true;
		Int = val;
	}

	bool getterWasCalled = false;
	int& IntGetter()
	{
		getterWasCalled = true;
		return Int;
	}

	char padding[2] = { '\0' };

private:
	int privateInt = 456;
};

REFLECT(ClassTest) < TestTag >,
	BASES < Base >,
	MEMBERS <
		REF_MEMBER(Int)< 
			Hidden,
			Setter<ClassTest, int, &ClassTest::IntSetter>, 
			Getter<ClassTest, int, &ClassTest::IntGetter>
		>,
		REF_MEMBER(floatPtr)<>,
		REF_MEMBER(padding)<>,
		REF_MEMBER(privateInt)<>
	>
REF_END;

enum class ValueField
{
	Val0 = 0,
	Val2 = 2,
	Val4 = 4,
	Val1024 = 1024,
	Val_First = Val0
};

REFLECT(ValueField) < BitField >,
	VALUES <
		REF_VALUE(Val0),
		REF_VALUE(Val2),
		REF_VALUE(Val4),
		REF_VALUE(Val1024),
		REF_VALUE(Val_First)
	>
REF_END;

TEST_CASE("Reflection")
{
	using namespace Meta;

	SECTION("Hash Function")
	{
		static_assert(HashCRC("int") == HashCRC("int"), "HashCRC() did not return the same value twice at compile-time.");
		static_assert(HashCRC("int") != HashCRC("char"), "HashCRC() returned the same value for two different inputs at compile-time.");

		std::vector<unsigned> hashes;
		hashes.push_back(HashCRC("int"));
		hashes.push_back(HashCRC("123"));
		hashes.push_back(HashCRC("321"));
		hashes.push_back(HashCRC("float"));
		hashes.push_back(HashCRC("unsigned"));
		hashes.push_back(HashCRC("double"));
		hashes.push_back(HashCRC("char"));
		hashes.push_back(HashCRC("byte"));
		hashes.push_back(HashCRC("string"));
		hashes.push_back(HashCRC("void"));
		hashes.push_back(HashCRC("int*"));
		hashes.push_back(HashCRC("char*"));
		hashes.push_back(HashCRC("float[]"));
		hashes.push_back(HashCRC("test"));
		hashes.push_back(HashCRC("std::string"));

		auto preSize = hashes.size();
		hashes.erase(std::unique(hashes.begin(), hashes.end()), hashes.end());
		auto postSize = hashes.size();

		// If the size didn't change, this means all the hashes were unique.
		CHECK(preSize == postSize);
	}

	SECTION("Inspecting Types")
	{
		auto type = Reflection::Type::Find("ClassTest");
		auto base = Reflection::Type::Find("Base");
		REQUIRE(type != nullptr);
		REQUIRE(base != nullptr);

		CHECK(type->IsDerivedFrom(*base));
		CHECK(type->IsDerivedFrom(*type));
		CHECK_FALSE(base->IsDerivedFrom(*type));

		CHECK(type->traits.size == sizeof(ClassTest));
		CHECK(strcmp(type->traits.name, "ClassTest") == 0);
		CHECK(type->traits.nameLen == strlen("ClassTest"));
		CHECK(type->traits.alignment == 4);
		CHECK(type->GetMembers().size() == 4);
		CHECK(type->traits.nameHash == HashCRC("ClassTest"));
		
		CHECK_FALSE(type->traits.isAbstract);
		CHECK_FALSE(type->traits.isFinal);
		CHECK_FALSE(type->traits.isEnum);
		CHECK_FALSE(type->traits.isFundamental);
		CHECK_FALSE(type->traits.isIntegral);
		CHECK_FALSE(type->traits.isFloatingPoint);
		CHECK_FALSE(type->traits.isUnsigned);
		CHECK_FALSE(type->traits.isPolymorhpic);
		CHECK_FALSE(type->traits.isEmpty);
	}

	SECTION("MetaTags")
	{
		auto type = Reflection::Type::Find("ClassTest");
		REQUIRE(type != nullptr);

		SECTION("Classes")
		{
			CHECK(type->HasMetaTag<Reflection::TestTag>());
			CHECK_FALSE(type->HasMetaTag<Reflection::BitField>());
		}

		SECTION("Members")
		{
			auto& member = (*type)["Int"];
			
			CHECK(member.HasMetaTag<Reflection::Hidden>());
			CHECK(member.HasMetaTag<Reflection::detail::Setter_Impl>());
			CHECK(member.HasMetaTag<Reflection::detail::Getter_Impl>());
			CHECK_FALSE(member.HasMetaTag<Reflection::TestTag>());
		}
	}

	SECTION("Inspecting Members")
	{
		auto type = Reflection::Type::Find("ClassTest");
		REQUIRE(type != nullptr);

		SECTION("Public")
		{
			auto& member = (*type)["Int"];

			CHECK(strcmp(member.traits.name, "Int") == 0);
			CHECK(member.traits.nameLen == strlen("Int"));
			CHECK(member.traits.offset == 0);
			CHECK(member.GetType() == Reflection::Type::Find<int>());
			CHECK(member.traits.arraySize == 1);

			CHECK_FALSE(member.traits.isArray);
			CHECK_FALSE(member.traits.isPointer);
			CHECK_FALSE(member.traits.isConst);
			CHECK_FALSE(member.traits.isReference);
			CHECK_FALSE(member.traits.isVolatile);
		}

		SECTION("Private")
		{
			auto& member = (*type)["privateInt"];

			CHECK(strcmp(member.traits.name, "privateInt") == 0);
			CHECK(member.traits.nameLen == strlen("privateInt"));
			CHECK(member.traits.offset == 12);
			CHECK(member.GetType() == Reflection::Type::Find<int>());
			CHECK(member.traits.arraySize == 1);

			CHECK_FALSE(member.traits.isArray);
			CHECK_FALSE(member.traits.isPointer);
			CHECK_FALSE(member.traits.isConst);
			CHECK_FALSE(member.traits.isReference);
			CHECK_FALSE(member.traits.isVolatile);
		}
	}

	SECTION("Inspecting Enums")
	{
		auto type = Reflection::Type::Find("ValueField");
		REQUIRE(type != nullptr);

		CHECK(type->traits.size == sizeof(ValueField));
		CHECK(strcmp(type->traits.name, "ValueField") == 0);
		CHECK(type->traits.alignment == 4);
		CHECK(type->traits.nameHash == Meta::HashCRC("ValueField"));
		CHECK(type->traits.isEnum);

		CHECK(type->HasMetaTag<Reflection::BitField>());

		CHECK_FALSE(type->traits.isFundamental);
		CHECK_FALSE(type->traits.isAbstract);
		CHECK_FALSE(type->traits.isPolymorhpic);
		CHECK_FALSE(type->traits.isFinal);
		CHECK_FALSE(type->traits.isAssignable);
		CHECK_FALSE(type->traits.isIntegral);
		CHECK_FALSE(type->traits.isFloatingPoint);
		CHECK_FALSE(type->traits.isUnsigned);
		CHECK_FALSE(type->traits.isEmpty);

		CHECK(type->GetEnumValues().size() == 5);
		CHECK(type->GetEnumValues().at("Val0") == 0);
		CHECK(type->GetEnumValues().at("Val2") == 2);
		CHECK(type->GetEnumValues().at("Val4") == 4);
		CHECK(type->GetEnumValues().at("Val1024") == 1024);
		CHECK(type->GetEnumValues().at("Val_First") == 0);
	}

	SECTION("Setters and Getters")
	{
		REQUIRE(Reflection::Type::Find("ClassTest") != nullptr);
		auto& type = *Reflection::Type::Find("ClassTest");

		auto& member = type["Int"];
		auto& privateMember = type["privateInt"];
			
		ClassTest object;

		CHECK_FALSE(object.getterWasCalled);

		CHECK(object.Int == 123);
		CHECK(member.get<int>(object) == 123);
		CHECK(privateMember.get<int>(object) == 456);

		CHECK(object.getterWasCalled);
		CHECK_FALSE(object.setterWasCalled);

		int newValue = -50;
		member.set(object, newValue);
		privateMember.set(object, newValue);
		
		CHECK(object.setterWasCalled);

		CHECK(object.Int == -50);
		CHECK(privateMember.get<int>(object) == -50);
	}
}
