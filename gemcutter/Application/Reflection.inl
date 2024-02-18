// Copyright (c) 2023 Emilian Cioca
namespace gem
{
	template<typename T> [[nodiscard]]
	const loupe::type& ReflectType()
	{
		if constexpr (std::is_same_v<std::remove_cv_t<T>, class Entity>)
		{
			return *EntityTypeId;
		}
		else if constexpr (std::is_same_v<std::remove_cv_t<T>, class ComponentBase>)
		{
			return *BaseComponentTypeId;
		}
		else if constexpr (std::is_same_v<std::remove_cv_t<T>, class ResourceBase>)
		{
			return *BaseResourceTypeId;
		}
		else
		{
			static const loupe::type* descriptor = reflection_tables.find<T>();
			ASSERT(descriptor, "The type was not reflected.");

			return *descriptor;
		}
	}

	template<typename Enum> [[nodiscard]]
	std::optional<Enum> StringToEnum(std::string_view valueName)
	{
		static_assert(std::is_enum_v<Enum>);

		static const loupe::type* descriptor = reflection_tables.find<Enum>();
		ASSERT(descriptor, "The enum type was not reflected.");

		const auto& enumeration = std::get<loupe::enumeration>(descriptor->data);
		const uint16_t* result = enumeration.find_value(valueName);

		return result ? std::optional<Enum>{static_cast<Enum>(*result)} : std::nullopt;
	}

	template<typename Enum> [[nodiscard]]
	std::string_view EnumToString(Enum value)
	{
		static_assert(std::is_enum_v<Enum>);

		static const loupe::type* descriptor = reflection_tables.find<Enum>();
		ASSERT(descriptor, "The enum type was not reflected.");

		const auto& enumeration = std::get<loupe::enumeration>(descriptor->data);
		const std::string_view* result = enumeration.find_name(static_cast<uint16_t>(value));
		ASSERT(result, "The enum value was not reflected.");

		return *result;
	}

	template<typename T> [[nodiscard]]
	consteval std::string_view GetTypeName()
	{
		return loupe::get_type_name<T>();
	}
}
