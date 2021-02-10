// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <type_traits>

namespace gem
{
	// Allows you to easily use a strongly-typed enum as if it was a bit-field.
	template<typename Enumeration, typename Primitive = std::underlying_type_t<Enumeration>>
	class EnumFlags
	{
	public:
		EnumFlags()
			: value(Primitive())
		{
		}

		EnumFlags(Enumeration val)
			: value(static_cast<Primitive>(val))
		{
		}

		EnumFlags(Primitive val)
			: value(val)
		{
		}

		void Clear()
		{
			value = Primitive();
		}

		bool Has(EnumFlags mask) const
		{
			return (value & mask.value) != Primitive();
		}

		bool operator==(Enumeration val) const
		{
			return value == static_cast<Primitive>(val);
		}

		bool operator==(Primitive val) const
		{
			return value == val;
		}

		EnumFlags& operator|=(Enumeration val)
		{
			value = value | static_cast<Primitive>(val);
			return *this;
		}

		EnumFlags& operator|=(Primitive val)
		{
			value = value | val;
			return *this;
		}

		EnumFlags& operator&=(Enumeration val)
		{
			value = value & static_cast<Primitive>(val);
			return *this;
		}

		EnumFlags& operator&=(Primitive val)
		{
			value = value & val;
			return *this;
		}

		EnumFlags operator|(Enumeration val) const
		{
			return value | static_cast<Primitive>(val);
		}

		EnumFlags operator|(Primitive val) const
		{
			return value | val;
		}

		EnumFlags operator&(Enumeration val) const
		{
			return value & static_cast<Primitive>(val);
		}

		EnumFlags operator&(Primitive val) const
		{
			return value & val;
		}

		operator Enumeration() const
		{
			return static_cast<Enumeration>(value);
		}

		operator Primitive() const
		{
			return value;
		}

	private:
		Primitive value;
	};
}
