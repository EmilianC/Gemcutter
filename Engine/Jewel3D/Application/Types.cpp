#include "Jewel3D/Application/Types.h"

namespace Jwl
{
	// Double-checks the size of the types

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