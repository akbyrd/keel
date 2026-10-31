#pragma once

#include "kl/containers/span.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

namespace kl
{
	// NOTE: FNV-1a is inherently serial (each byte depends on the previous result) so it cannot be
	// vectorized. It's meant for short keys and compile time hashing. A SIMD hash would be a separate
	// algorithm with the same shape.
	// NOTE: Pass the result of a previous hash as the seed to hash multiple spans as if they were
	// contiguous.
	constexpr u64 hashFnv1a(Span<const u8> bytes);
	constexpr u64 hashFnv1a(Span<const u8> bytes, u64 seed);
}

// -------------------------------------------------------------------------------------------------
// Implementation

namespace kl
{
	constexpr u64 fnv1aOffsetBasis = 0xCBF2'9CE4'8422'2325;
	constexpr u64 fnv1aPrime       = 0x0000'0100'0000'01B3;

	constexpr u64
	hashFnv1a(Span<const u8> bytes)
	{
		return hashFnv1a(bytes, fnv1aOffsetBasis);
	}

	constexpr u64
	hashFnv1a(Span<const u8> bytes, u64 seed)
	{
		u64 hash = seed;
		for (u8 x : bytes)
			hash = (hash ^ x) * fnv1aPrime;
		return hash;
	}
}
