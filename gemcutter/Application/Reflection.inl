// Copyright (c) 2023 Emilian Cioca
namespace gem
{
	// Returns the reflection information for the given type.
	template<typename T> [[nodiscard]]
	const loupe::type& ReflectType()
	{
		static const loupe::type* descriptor = reflection_tables.find<T>();
		ASSERT(descriptor, "The type was not reflected.");

		return *descriptor;
	}

	template<typename Enum> [[nodiscard]]
	std::optional<Enum> StringToEnum(std::string_view valueName)
	{
		static_assert(std::is_enum_v<Enum>);

		static const loupe::type* descriptor = reflection_tables.find<Enum>();
		ASSERT(descriptor, "The enum type was not reflected.");

		const auto& enumeration = std::get<loupe::enumeration>(descriptor->data);
		const std::size_t* result = enumeration.find_value(valueName);

		return result ? std::optional<Enum>{static_cast<Enum>(*result)} : std::nullopt;
	}

	template<typename Enum> [[nodiscard]]
	std::string_view EnumToString(Enum value)
	{
		static_assert(std::is_enum_v<Enum>);

		static const loupe::type* descriptor = reflection_tables.find<Enum>();
		ASSERT(descriptor, "The enum type was not reflected.");

		const auto& enumeration = std::get<loupe::enumeration>(descriptor->data);
		const std::string_view* result = enumeration.find_name(static_cast<std::size_t>(value));
		ASSERT(result, "The enum value was not reflected.");

		return *result;
	}

	// Returns a string representation of the given type's name.
	// Results will vary by compiler and are best used for debug purposes only.
	template<typename T> [[nodiscard]]
	consteval std::string_view GetTypeName()
	{
		return loupe::get_type_name<T>();
	}
}
