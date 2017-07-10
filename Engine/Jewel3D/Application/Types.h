#pragma once

// Required for std::numeric_limits<>
#include <limits>

// Type definitions
#include <stdint.h>

namespace Jwl
{
	//- Integers
	using u8 = uint8_t;
	using s8 = int8_t;

	using u16 = uint16_t;
	using s16 = int16_t;

	using u32 = uint32_t;
	using s32 = int32_t;

	using u64 = uint64_t;
	using s64 = int64_t;

	//- Floating-point
	using f32 = float;
	using f64 = double;

	//- Chars (important for C++11 and Unicode)
	using c16 = char16_t;
	using c32 = char32_t;

	/////////////////
	//- Sanity checks
	static_assert(sizeof(u8) == 1, "u8 size is not 1 byte.");
	static_assert(sizeof(s8) == 1, "s8 size is not 1 byte.");

	static_assert(sizeof(u16) == 2, "u8 size is not 2 bytes.");
	static_assert(sizeof(s16) == 2, "s8 size is not 2 bytes.");

	static_assert(sizeof(u32) == 4, "u32 size is not 4 bytes.");
	static_assert(sizeof(s32) == 4, "s32 size is not 4 bytes.");

	static_assert(sizeof(u64) == 8, "u64 size is not 8 bytes.");
	static_assert(sizeof(s64) == 8, "s64 size is not 8 bytes.");

	static_assert(sizeof(f32) == 4, "f32 size is not 4 bytes.");
	static_assert(sizeof(f64) == 8, "f64 size is not 8 bytes.");

	static_assert(sizeof(c16) == 2, "c32 size is not 2 bytes.");
	static_assert(sizeof(c32) == 4, "c64 size is not 4 bytes.");
}