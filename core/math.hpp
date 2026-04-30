#pragma once

#include "core/debug.hpp"
#include "core/traits.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

// TODO: How are documentation comments handled with this split?
// TODO: Parse and generate documentation
// TODO: Tests that verify the interface + implementation split
// TODO: Test negative integers
// TODO: Test integer promotion
// TODO: void* or u8*?

template <typename T> requires (isUnsigned<T>) constexpr u8 countSetBits(T x);
template <typename T> requires (isIntegral<T>) constexpr b8 isPowerOf2(T x);
template <typename T> requires (isIntegral<T>) constexpr b8 isMultipleOf(T x, T multipleOf);
template <typename T> requires (isIntegral<T>) constexpr T  roundUp(T x, T round);
template <typename T> requires (isIntegral<T>) constexpr T  roundDown(T x, T round);

template <typename T> constexpr T min(T x, T y);
template <typename T> constexpr T max(T x, T y);

template <typename T> constexpr T*  alignUp(T* x);
template <typename T> constexpr u8* alignUp(u8* x);
template <typename T> constexpr T*  alignDown(T* x);
template <typename T> constexpr u8* alignDown(u8* x);

// TODO: Maybe these should be in a struct? Something like attr<T>?
template <typename T> requires (isArithmetic<T>)    constexpr T minVal;
template <typename T> requires (isArithmetic<T>)    constexpr T maxVal;
template <typename T> requires (isFloatingPoint<T>) constexpr T smallestVal;

template <typename T> requires (isFloatingPoint<T>) constexpr T pi;
template <typename T> requires (isFloatingPoint<T>) constexpr T tau;
template <typename T> requires (isFloatingPoint<T>) constexpr T goldenRatio;

// -------------------------------------------------------------------------------------------------
// Implementation

// NOTE: Zero is not a power of 2
template <typename T>
	requires (isIntegral<T>)
constexpr b8 isPowerOf2(T x)
{
	b8 result = countSetBits(x) == 1;
	return result;
}

template <typename T>
	requires (isIntegral<T>)
constexpr b8 isMultipleOf(T x, T multipleOf)
{
	T rounded = roundDown(x, multipleOf);
	b8 result = rounded == x;
	return result;
}

// NOTE: rounding up may overflow
template <typename T>
	requires (isIntegral<T>)
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

	T result = (x + round - 1) / round * round;
	return result;
}

template <typename T>
	requires (isIntegral<T>)
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

	T result = x / round * round;
	return result;
}

template <typename T>
constexpr T min(T x, T y)
{
	return x < y ? x : y;
}

template <typename T>
constexpr T max(T x, T y)
{
	return x > y ? x : y;
}

// TODO: Try removing this overload
// NOTE: aligning up may overflow
template <typename T>
constexpr u8* alignUp(u8* x)
{
	// NOTE: [basic.align] - alignment is always a power of 2 which is required here.
	constexpr u64 align = alignof(T);
	static_assert(isPowerOf2(align));

	u8* result = x + (-u64(x) & (align - 1));
	return result;
}

// NOTE: aligning up may overflow
template <typename T>
constexpr T* alignUp(T* x)
{
	T* result = static_cast<T*>(alignUp<T>(static_cast<u8*>(x)));
	return result;
}

template <typename T>
constexpr u8* alignDown(u8* x)
{
	// NOTE: [basic.align] - alignment is always a power of 2 which is required here.
	constexpr u64 align = alignof(T);
	static_assert(isPowerOf2(align));

	u8* result = reinterpret_cast<u8*>(u64(x) & ~(align - 1));
	return result;
}

template <typename T>
constexpr T* alignDown(T* x)
{
	T* result = static_cast<T*>(alignDown<T>(static_cast<u8*>(x)));
	return result;
}

template <> constexpr u8  minVal<u8>  = 0;
template <> constexpr u16 minVal<u16> = 0;
template <> constexpr u32 minVal<u32> = 0;
template <> constexpr u64 minVal<u64> = 0;
template <> constexpr i8  minVal<i8>  = static_cast<i8>(0x80);
template <> constexpr i16 minVal<i16> = static_cast<i16>(0x8000);
template <> constexpr i32 minVal<i32> = 0x8000'0000;
template <> constexpr i64 minVal<i64> = 0x8000'0000'0000'0000;
template <> constexpr f32 minVal<f32> = f32(0xFF7F'FFFF);
template <> constexpr f64 minVal<f64> = f64(0xFFEF'FFFF'FFFF'FFFF);
template <> constexpr c8  minVal<c8>  = isSigned<c8> ? c8(0x80) : 0;

template <> constexpr u8  maxVal<u8>  = 0xFF;
template <> constexpr u16 maxVal<u16> = 0xFFFF;
template <> constexpr u32 maxVal<u32> = 0xFFFF'FFFF;
template <> constexpr u64 maxVal<u64> = 0xFFFF'FFFF'FFFF'FFFF;
template <> constexpr i8  maxVal<i8>  = 0x7F;
template <> constexpr i16 maxVal<i16> = 0x7FFF;
template <> constexpr i32 maxVal<i32> = 0x7FFF'FFFF;
template <> constexpr i64 maxVal<i64> = 0x7FFF'FFFF'FFFF'FFFF;
template <> constexpr f32 maxVal<f32> = f32(0x7F7F'FFFF);
template <> constexpr f64 maxVal<f64> = f64(0x7FEF'FFFF'FFFF'FFFF);
template <> constexpr c8  maxVal<c8>  = isSigned<c8> ? 0x7F : 0xFF;

// TODO: Consider a "make float" function
template <> constexpr f32 smallestVal<f32> = 0x0080'0000;
template <> constexpr f64 smallestVal<f64> = 0x0010'0000'0000'0000;

template <> constexpr f32 pi<f32>          = 3.14159265358979323f;
template <> constexpr f64 pi<f64>          = 3.14159265358979323;
template <> constexpr f32 tau<f32>         = 6.28318530717958647f;
template <> constexpr f64 tau<f64>         = 6.28318530717958647;
template <> constexpr f32 goldenRatio<f32> = 1.61803398874989484f;
template <> constexpr f64 goldenRatio<f64> = 1.61803398874989484;

#include "core/math.win32.hpp" // IWYU pragma: export

// -------------------------------------------------------------------------------------------------
// Tests

#if false
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
#endif
