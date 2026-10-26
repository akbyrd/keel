#pragma once

#include "containers/span.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

namespace kl
{

template <typename T> constexpr Span<u8> toBytes(T& x);

constexpr void clearBytes (Span<u8> dst);
constexpr void fillBytes  (Span<u8> dst, u8 src);
constexpr void copyBytes  (Span<u8> dst, Span<const u8> src);
constexpr void moveBytes  (Span<u8> dst, Span<u8> src);

}

// -------------------------------------------------------------------------------------------------
// Implementation

namespace kl
{

template <typename T>
constexpr Span<u8>
toBytes(T& x)
{
	return { sizeof(x), &x };
}

}

#include "memory/bytes.msvc.hpp" // IWYU pragma: export
