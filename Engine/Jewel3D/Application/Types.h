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
}