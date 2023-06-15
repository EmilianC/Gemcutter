// Copyright (c) 2022 Emilian Cioca
#pragma once
#include "gemcutter/Application/Logging.h"

#include <loupe/loupe.h>
#include <optional>

namespace gem
{
	// Reflection metadata Tags.
	struct ReadOnly {};
	struct Hidden {};

	extern loupe::reflection_blob reflection_tables;

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

#define REFLECT_RESOURCE_SIMPLE(resource) REFLECT(resource) BASES { REF_BASE(gem::ResourceBase) } REF_END;
#define REFLECT_COMPONENT_SIMPLE(component) REFLECT(component) BASES { REF_BASE(gem::ComponentBase) } REF_END;

#include "Reflection.inl"
