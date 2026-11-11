#pragma once

#include "kl/core/bits.hpp"

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
