// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <type_traits>

namespace gem
{
	// Allows you to easily use a strongly-typed enum as if it was a bit-field.
	template<typename Enumeration>
	class EnumFlags
	{
	public:
		using PrimitiveType = std::underlying_type_t<Enumeration>;

		EnumFlags()                  : value(PrimitiveType{}) {}
		EnumFlags(Enumeration val)   : value(static_cast<PrimitiveType>(val)) {}
		EnumFlags(PrimitiveType val) : value(val) {}

		void Clear()
		{
			value = PrimitiveType{};
		}

		[[nodiscard]]
		PrimitiveType Value() const
		{
			return value;
		}

		[[nodiscard]]
		bool Has(EnumFlags mask) const
		{
			return (value & mask.value) != PrimitiveType{};
		}

		[[nodiscard]]
		bool operator==(EnumFlags other) const
		{
			return value == other.value;
		}

		[[nodiscard]]
		bool operator!=(EnumFlags other) const
		{
			return value != other.value;
		}

		EnumFlags& operator|=(EnumFlags other)
		{
			value = value | other.value;
			return *this;
		}

		EnumFlags& operator&=(EnumFlags other)
		{
			value = value & other.value;
			return *this;
		}

		[[nodiscard]]
		EnumFlags operator|(EnumFlags other) const
		{
			return value | other.value;
		}

		[[nodiscard]]
		EnumFlags operator&(EnumFlags other) const
		{
			return value & other.value;
		}

		[[nodiscard]]
		explicit operator Enumeration() const
		{
			return static_cast<Enumeration>(value);
		}

		[[nodiscard]]
		explicit operator PrimitiveType() const
		{
			return value;
		}

	private:
		PrimitiveType value;
	};
}
