// Copyright (c) 2022 Emilian Cioca
#pragma once
#include "gemcutter/Application/Logging.h"

#include <loupe/loupe.h>
#include <loupe/metadata.h>
#include <optional>

namespace gem
{
	// The core reflection data for the whole program.
	extern loupe::reflection_blob reflection_tables;

	// Common typeIds cached for use in quick comparisons. Will never be null.
	extern const loupe::type* EntityTypeId;
	extern const loupe::type* BaseComponentTypeId;
	extern const loupe::type* BaseResourceTypeId;

	// Populates the global reflection tables for the program.
	// Should only be called once at the top of main().
	void InitializeReflectionTables();

	// Returns the reflection information for the given type.
	template<typename T> [[nodiscard]]
	const loupe::type& ReflectType();

	// Uses reflection information to convert the name into the corresponding enum value.
	template<typename Enum> [[nodiscard]]
	std::optional<Enum> StringToEnum(std::string_view valueName);

	// Uses reflection information to convert the enum value into the corresponding string name.
	// Asserts if the value is not properly reflected.
	template<typename Enum> [[nodiscard]]
	std::string_view EnumToString(Enum value);

	// Returns a string representation of the given type's name.
	// Results will vary by compiler and are best used for debug purposes only.
	template<typename T> [[nodiscard]]
	consteval std::string_view GetTypeName();
}

#define REFLECT_RESOURCE(resource)         REFLECT(resource)  BASES { REF_BASE(gem::ResourceBase) }
#define REFLECT_COMPONENT(component, base) REFLECT(component) BASES { REF_BASE(base) } USER_CONSTRUCTOR(gem::Entity&)

#include "Reflection.inl"
