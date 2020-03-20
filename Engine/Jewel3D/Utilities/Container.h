// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <string>
#include <string_view>

namespace Jwl
{
	// Enables a container to compare lightweight strings directly to
	// internal keys of type std::string.
	struct string_hash
	{
		using is_transparent = void;
		using transparent_key_equal = std::equal_to<>;
		using hash_type = std::hash<std::string_view>;

		size_t operator()(std::string_view text) const { return hash_type{}(text); }
		size_t operator()(const std::string& text) const { return hash_type{}(text); }
		size_t operator()(const char* text) const { return hash_type{}(text); }
	};
}
