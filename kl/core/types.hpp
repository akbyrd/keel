#pragma once

namespace kl
{
	using u8  = unsigned char;
	using u16 = unsigned short;
	using u32 = unsigned int;
	using u64 = decltype(0xFFFFFFFFFFFFFFFF);
	using ux  = decltype(sizeof(0));

	using i8  = signed char;
	using i16 = signed short;
	using i32 = signed int;
	using i64 = decltype(0x7FFFFFFFFFFFFFFF);
	using ix  = decltype((u8*) 0 - (u8*) 0);

	using f32 = float;
	using f64 = double;

	using b8 = bool;

	using c8    = char;
	using cw    = wchar_t;
	using utf8  = char8_t;
	using utf16 = char16_t;
	using utf32 = char32_t;

	using null = decltype(nullptr);
}

// -------------------------------------------------------------------------------------------------
// Tests

#include "kl/core/traits.hpp"
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
