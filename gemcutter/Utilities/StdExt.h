// Copyright (c) 2022 Emilian Cioca
#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <variant>

namespace gem
{
	// Enables a standard container to compare lightweight strings
	// directly to internal keys of type std::string.
	struct string_hash
	{
		using is_transparent = void;
		using transparent_key_equal = std::equal_to<>;
		using hash_type = std::hash<std::string_view>;

		size_t operator()(std::string_view text) const   { return hash_type{}(text); }
		size_t operator()(const std::string& text) const { return hash_type{}(text); }
		size_t operator()(const char* text) const        { return hash_type{}(text); }
	};

	// Returns true if the pointer is explicitly null or default-initialized.
	template<class T> [[nodiscard]]
	bool IsPtrNull(const std::weak_ptr<T>& ptr)
	{
		std::weak_ptr<void> empty;
		return !ptr.owner_before(empty) && !empty.owner_before(ptr);
	}

	// Helper for visiting all the alternatives in a variant with lambdas.
	template<typename Variant, typename... Visitors>
	auto Visit(Variant&& variant, Visitors&&... visitors)
	{
		struct Overload : public Visitors...
		{
			using Visitors::operator()...;
		};
	
		return std::visit(Overload { std::forward<Visitors>(visitors)... }, std::forward<Variant>(variant));
	}
}
