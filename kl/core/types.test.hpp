#pragma once

#include "kl/core/traits.hpp"
#include "kl/core/types.hpp"
#include <climits>

namespace kl
{
	static_assert(isAnyOf<ux, u32, u64>);
	static_assert(sizeof(u8) == 1);
	static_assert(sizeof(u16) == 2);
	static_assert(sizeof(u32) == 4);
	static_assert(sizeof(u64) == 8);

	static_assert(isAnyOf<ix, i32, i64>);
	static_assert(sizeof(i8) == 1);
	static_assert(sizeof(i16) == 2);
	static_assert(sizeof(i32) == 4);
	static_assert(sizeof(i64) == 8);

	static_assert(sizeof(f32) == 4);
	static_assert(sizeof(f64) == 8);

	static_assert(sizeof(b8) == 1);

	static_assert(CHAR_BIT == 8);
	static_assert(sizeof(c8)    == 1);
	static_assert(sizeof(utf8)  == 1);
	static_assert(sizeof(utf16) == 2);
	static_assert(sizeof(utf32) == 4);
}
