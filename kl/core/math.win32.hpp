#pragma once

#include "kl/core/platform.hpp"

#if KL_PLATFORM_WIN32

#include "kl/core/math.hpp"
#include <intrin.h>

namespace kl
{
	template <typename T>
		requires (isUnsignedInteger<T>)
	constexpr u8 countSetBits(T x)
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
}

#endif

// -------------------------------------------------------------------------------------------------
// Tests

namespace kl
{
	static constexpr b8
	test_countSetBits()
	{
		#define TEST(x) \
		{ \
			if (isConstantEvaluated()) \
			{ \
				static_assert(x, #x); \
			} \
			else \
			{ \
				if (!(x)) \
				{ \
					return false; \
				} \
			} \
		}

		TEST(countSetBits(u8(0b0000'0000)) == 0);
		TEST(countSetBits(u8(0b1111'1111)) == 8);
		TEST(countSetBits(u8(0b0000'0001)) == 1);
		TEST(countSetBits(u8(0b0000'0010)) == 1);
		TEST(countSetBits(u8(0b0000'0100)) == 1);
		TEST(countSetBits(u8(0b0000'1000)) == 1);
		TEST(countSetBits(u8(0b0001'0000)) == 1);
		TEST(countSetBits(u8(0b0010'0000)) == 1);
		TEST(countSetBits(u8(0b0100'0000)) == 1);
		TEST(countSetBits(u8(0b1000'0000)) == 1);

		TEST(countSetBits(u16(0b0000'0000'0000'0000)) == 0);
		TEST(countSetBits(u16(0b1111'1111'1111'1111)) == 16);
		TEST(countSetBits(u16(0b1110'1101'1011'0111)) == 12);

		TEST(countSetBits(u32(0b0000'0000'0000'0000'0000'0000'0000'0000)) == 0);
		TEST(countSetBits(u32(0b1111'1111'1111'1111'1111'1111'1111'1111)) == 32);
		TEST(countSetBits(u32(0b1100'1010'1001'0110'0101'0011'0000'0000)) == 12);

		TEST(countSetBits(u64(0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000)) == 0);
		TEST(countSetBits(u64(0b1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111)) == 64);
		TEST(countSetBits(u64(0b1111'1110'1101'1100'1011'1010'1001'1000'0111'0110'0101'0100'0011'0010'0001'0000)) == 32);

		return true;
		#undef TEST
	}
}
