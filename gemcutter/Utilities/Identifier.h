// Copyright (c) 2022 Emilian Cioca
#pragma once
#include "Gemcutter/Application/Logging.h"

#include <limits>
#include <type_traits>

namespace gem
{
	// A base class for strongly-typed identifiers.
	// The intended usage is to derive from this structure:
	//
	// struct CustomId : public Identifier<char> {};
	// CustomId id = GenerateUniqueId<CustomId>();
	//
	// Please note that you will need to specialize std::hash<>
	// if you wish to use your identifiers as keys in containers.
	template<typename T = unsigned>
	struct Identifier
	{
		static_assert(std::is_integral_v<T>);
		using ValueType = T;

		ValueType GetValue() const { return value; }
		void Reset() { value = invalid; }

		[[nodiscard]] bool IsValid() const { return value != invalid; }
		[[nodiscard]] bool operator==(const Identifier&) const = default;
		[[nodiscard]] bool operator!=(const Identifier&) const = default;

		static constexpr ValueType invalid = std::numeric_limits<ValueType>::min();

	private:
		template<typename IdType> friend IdType GenerateUniqueId();
		ValueType value = invalid;
	};

	template<typename IdType>
	IdType GenerateUniqueId()
	{
		static IdType::ValueType counter = IdType::invalid;

		ASSERT(counter != std::numeric_limits<typename IdType::ValueType>::max(),
			"The underlying identifier type ran out of unique values to generate.");

		IdType Id;
		Id.value = ++counter;
		return Id;
	}
}
