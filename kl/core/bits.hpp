#pragma once

#include "kl/core/traits.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

namespace kl
{
	template <typename T, typename U> concept isReinterpretable = sizeof(T) == sizeof(U) && isTriviallyCopyable<T> && isTriviallyCopyable<U>;

	template <typename T, typename U> requires (isReinterpretable<T, U>) constexpr T bitCast(const U& x);
	template <typename T> requires (isUnsignedInteger<T>) constexpr u8 countSetBits(T x);
}

// -------------------------------------------------------------------------------------------------
// Implementation

#include "kl/core/compiler.hpp"

namespace kl
{
	template <typename T, typename U>
		requires (isReinterpretable<T, U>)
	constexpr T
	bitCast(const U& x)
	{
		return __builtin_bit_cast(T, x);
	}

	#if KL_COMPILER_MSVC
	extern "C"
	{
		u16 __popcnt16(u16);
		u32 __popcnt(u32);
		u64 __popcnt64(u64);
	}

	template <typename T>
		requires (isUnsignedInteger<T>)
	constexpr u8
	countSetBits(T x)
	{
		static_assert(sizeof(T) <= 8, "Size of type is not supported");

		if (isConstantEvaluated())
		{
			// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
			// https://en.wikipedia.org/wiki/Hamming_weight
			constexpr T h01 = T(~T(0)) / 0xFF;
			constexpr T m1  = h01 * 0b0101'0101;
			constexpr T m2  = h01 * 0b0011'0011;
			constexpr T m4  = h01 * 0b0000'1111;

			x = T( x       - ((x >> 1)  & m1));
			x = T((x & m2) + ((x >> 2)  & m2));
			x = T((x       +  (x >> 4)) & m4);
			u8 count = u8((x * h01) >> (sizeof(T) - 1) * 8);
			return count;
		}
		else
		{
			if constexpr (sizeof(T) <= 2)
			{
				u8 result = u8(__popcnt16(x));
				return result;
			}
			else if constexpr (sizeof(T) <= 4)
			{
				u8 result = u8(__popcnt(x));
				return result;
			}
			else if constexpr (sizeof(T) <= 8)
			{
				u8 result = u8(__popcnt64(x));
				return result;
			}
		}
	}

	#else
	template <typename T>
		requires (isUnsignedInteger<T>)
	constexpr u8
	countSetBits(T x)
	{
		static_assert(sizeof(T) <= 8, "Size of type is not supported");

		u8 result = u8(__builtin_popcountg(x));
		return result;
	}
	#endif
}
