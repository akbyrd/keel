#pragma once

#include "kl/core/debug.hpp"
#include "kl/core/traits.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

namespace kl
{
	// TODO: Test negative integers
	// TODO: Test integer promotion
	// TODO: Test and document special cases (NaN, Inf, inverted bounds, overflow, underflow, etc)
	// TODO: Implement safe and unsafe versions

	template <typename T> requires (isUnsignedInteger<T>) constexpr u8 countSetBits(T x);
	template <typename T> requires (isInteger<T>) constexpr b8 isPowerOf2(T x);
	template <typename T> requires (isInteger<T>) constexpr b8 isMultipleOf(T x, T multipleOf);
	template <typename T> requires (isInteger<T>) constexpr T  roundUp(T x, T round);
	template <typename T> requires (isInteger<T>) constexpr T  roundDown(T x, T round);

	template <typename T> constexpr T min(T x, T y);
	template <typename T> constexpr T max(T x, T y);
	template <typename T> constexpr T clamp(T x, T min, T max);

	// TODO: Consider a constexpr offset version of alignUp/Down for easier testing

	// NOTE: align must be a power of 2
	// NOTE: aligning up may overflow
	u8*       alignUp(u8* x, u64 align);
	const u8* alignUp(const u8* x, u64 align);
	template <typename T> T*       alignUp(u8* x, u64 align = alignof(T));
	template <typename T> const T* alignUp(const u8* x, u64 align = alignof(T));

	u8*       alignDown(u8* x, u64 align);
	const u8* alignDown(const u8* x, u64 align);
	template <typename T> T*       alignDown(u8* x, u64 align = alignof(T));
	template <typename T> const T* alignDown(const u8* x, u64 align = alignof(T));

	// TODO: Maybe these should be in a struct? Something like attr<T>?
	template <typename T> requires (isArithmetic<T>) constexpr T minVal;
	template <typename T> requires (isArithmetic<T>) constexpr T maxVal;
	template <typename T> requires (isFloat<T>) constexpr T smallestVal;
	template <typename T> requires (isFloat<T>) constexpr T smallestDenormVal;

	template <typename T> requires (isFloat<T>) constexpr T pi;
	template <typename T> requires (isFloat<T>) constexpr T tau;
	template <typename T> requires (isFloat<T>) constexpr T goldenRatio;
}

// -------------------------------------------------------------------------------------------------
// Implementation

namespace kl
{
	// NOTE: Zero is not a power of 2
	template <typename T>
		requires (isInteger<T>)
	constexpr b8 isPowerOf2(T x)
	{
		b8 result = x > 0 && (x & (x - 1)) == 0;
		return result;
	}

	template <typename T>
		requires (isInteger<T>)
	constexpr b8 isMultipleOf(T x, T multipleOf)
	{
		T rounded = roundDown(x, multipleOf);
		b8 result = rounded == x;
		return result;
	}

	// NOTE: rounding up may overflow
	template <typename T>
		requires (isInteger<T>)
	constexpr T roundUp(T x, T round)
	{
		assert(round != 0);

		#if false
		if (isConstantEvaluated())
		{
			if constexpr (isPowerOf2(round))
			{
				T result = (x + round - 1) & ~(round - 1);
				return result;
			}
		}
		#endif

		T result = T((x + round - 1) / round * round);
		return result;
	}

	template <typename T>
		requires (isInteger<T>)
	constexpr T roundDown(T x, T round)
	{
		assert(round != 0);

		#if false
		if (isConstantEvaluated())
		{
			if constexpr (isPowerOf2(round))
			{
				T result = x & ~(round - 1);
				return result;
			}
		}
		#endif

		T result = T(x / round * round);
		return result;
	}

	template <typename T>
	constexpr T min(T x, T y)
	{
		// NOTE: May propagate NaN
		return x < y ? x : y;
	}

	template <typename T>
	constexpr T max(T x, T y)
	{
		// NOTE: May propagate NaN
		return x > y ? x : y;
	}

	template <typename T>
	constexpr T clamp(T x, T min, T max)
	{
		// NOTE: May propagate NaN
		// NOTE: Does not handle inverted bounds
		return x <= min ? min : x >= max ? max : x;
	}

	inline u8*
	alignUp(u8* x, u64 align)
	{
		assert(isPowerOf2(align));
		u8* result = x + ((0 - u64(x)) & (align - 1));
		return result;
	}

	inline const u8*
	alignUp(const u8* x, u64 align)
	{
		assert(isPowerOf2(align));
		const u8* result = x + ((0 - u64(x)) & (align - 1));
		return result;
	}

	template <typename T>
	T*
	alignUp(u8* x, u64 align)
	{
		u8* result = alignUp(x, align);
		return reinterpret_cast<T*>(result);
	}

	template <typename T>
	const T*
	alignUp(const u8* x, u64 align)
	{
		const u8* result = alignUp(x, align);
		return reinterpret_cast<const T*>(result);
	}

	inline u8*
	alignDown(u8* x, u64 align)
	{
		assert(isPowerOf2(align));
		u8* result = x - (u64(x) & (align - 1));
		return result;
	}

	inline const u8*
	alignDown(const u8* x, u64 align)
	{
		assert(isPowerOf2(align));
		const u8* result = x - (u64(x) & (align - 1));
		return result;
	}

	template <typename T>
	T*
	alignDown(u8* x, u64 align)
	{
		u8* result = alignDown(x, align);
		return reinterpret_cast<T*>(result);
	}

	template <typename T>
	const T*
	alignDown(const u8* x, u64 align)
	{
		const u8* result = alignDown(x, align);
		return reinterpret_cast<const T*>(result);
	}

	template <> constexpr inline u8  minVal<u8>  = 0;
	template <> constexpr inline u16 minVal<u16> = 0;
	template <> constexpr inline u32 minVal<u32> = 0;
	template <> constexpr inline u64 minVal<u64> = 0;
	template <> constexpr inline i8  minVal<i8>  = static_cast<i8>(0x80);
	template <> constexpr inline i16 minVal<i16> = static_cast<i16>(0x8000);
	template <> constexpr inline i32 minVal<i32> = 0x8000'0000;
	template <> constexpr inline i64 minVal<i64> = 0x8000'0000'0000'0000;
	template <> constexpr inline f32 minVal<f32> = __builtin_bit_cast(f32, 0xFF7F'FFFF);
	template <> constexpr inline f64 minVal<f64> = __builtin_bit_cast(f64, 0xFFEF'FFFF'FFFF'FFFF);
	template <> constexpr inline c8  minVal<c8>  = isSigned<c8> ? static_cast<c8>(0x80) : 0;

	template <> constexpr inline u8  maxVal<u8>  = 0xFF;
	template <> constexpr inline u16 maxVal<u16> = 0xFFFF;
	template <> constexpr inline u32 maxVal<u32> = 0xFFFF'FFFF;
	template <> constexpr inline u64 maxVal<u64> = 0xFFFF'FFFF'FFFF'FFFF;
	template <> constexpr inline i8  maxVal<i8>  = 0x7F;
	template <> constexpr inline i16 maxVal<i16> = 0x7FFF;
	template <> constexpr inline i32 maxVal<i32> = 0x7FFF'FFFF;
	template <> constexpr inline i64 maxVal<i64> = 0x7FFF'FFFF'FFFF'FFFF;
	template <> constexpr inline f32 maxVal<f32> = __builtin_bit_cast(f32, 0x7F7F'FFFF);
	template <> constexpr inline f64 maxVal<f64> = __builtin_bit_cast(f64, 0x7FEF'FFFF'FFFF'FFFF);
	template <> constexpr inline c8  maxVal<c8>  = isSigned<c8> ? 0x7F : 0xFF;

	// TODO: Consider a "make float" function
	template <> constexpr inline f32 smallestVal<f32>       = __builtin_bit_cast(f32, 0x0080'0000);
	template <> constexpr inline f64 smallestVal<f64>       = __builtin_bit_cast(f64, 0x0010'0000'0000'0000);
	template <> constexpr inline f32 smallestDenormVal<f32> = __builtin_bit_cast(f32, u32(0x0000'0001));
	template <> constexpr inline f64 smallestDenormVal<f64> = __builtin_bit_cast(f64, u64(0x0000'0000'0000'0001));
	template <> constexpr inline f32 pi<f32>                = 3.14159265358979323f;
	template <> constexpr inline f64 pi<f64>                = 3.14159265358979323;
	template <> constexpr inline f32 tau<f32>               = 6.28318530717958647f;
	template <> constexpr inline f64 tau<f64>               = 6.28318530717958647;
	template <> constexpr inline f32 goldenRatio<f32>       = 1.61803398874989484f;
	template <> constexpr inline f64 goldenRatio<f64>       = 1.61803398874989484;
}

#include "kl/core/math.win32.hpp" // IWYU pragma: export

// -------------------------------------------------------------------------------------------------
// Tests

#if false

namespace kl
{
	static_assert(alignUp<int>(0)     == 0);
	static_assert(alignUp<int>(1)     == 8);
	static_assert(alignUp<int>(7)     == 8);
	static_assert(alignUp<int>(8)     == 8);
	static_assert(alignUp<int>(0 - 1) == 0);
	static_assert(alignUp<int>(0 - 7) == 0);
	static_assert(alignUp<int>(0 - 8) == 0 - 8);

	static_assert(alignUp<int>((int*) 0)       == 0);
	static_assert(alignUp<int>((int*) 1)       == 8);
	static_assert(alignUp<int>((int*) 7)       == 8);
	static_assert(alignUp<int>((int*) 8)       == 8);
	static_assert(alignUp<int>((int*) (0 - 1)) == 0);
	static_assert(alignUp<int>((int*) (0 - 7)) == 0);
	static_assert(alignUp<int>((int*) (0 - 8)) == 0 - 8);

	static_assert(alignDown<int>(0)     == 0);
	static_assert(alignDown<int>(1)     == 0);
	static_assert(alignDown<int>(7)     == 0);
	static_assert(alignDown<int>(8)     == 0);
	static_assert(alignDown<int>(0 - 1) == 0 - 8);
	static_assert(alignDown<int>(0 - 7) == 0 - 8);
	static_assert(alignDown<int>(0 - 8) == 0 - 8);

	static_assert(alignDown<int>((int*) (0))     == 0);
	static_assert(alignDown<int>((int*) (1))     == 0);
	static_assert(alignDown<int>((int*) (7))     == 0);
	static_assert(alignDown<int>((int*) (8))     == 0);
	static_assert(alignDown<int>((int*) (0 - 1)) == 0 - 8);
	static_assert(alignDown<int>((int*) (0 - 7)) == 0 - 8);
	static_assert(alignDown<int>((int*) (0 - 8)) == 0 - 8);
}

#endif
