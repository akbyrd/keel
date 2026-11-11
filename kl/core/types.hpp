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
