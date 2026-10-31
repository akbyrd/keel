#pragma once

#include "kl/containers/span.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

namespace kl
{
	template <typename T> requires (!isConst<T>) constexpr Span<u8> toBytes(T& x);
	template <typename T>                        constexpr Span<const u8> toBytes(const T& x);

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
		requires (!isConst<T>)
	constexpr Span<u8>
	toBytes(T& x)
	{
		return { i64(sizeof(x)), reinterpret_cast<u8*>(&x) };
	}

	template <typename T>
	constexpr Span<const u8>
	toBytes(const T& x)
	{
		return { i64(sizeof(x)), reinterpret_cast<const u8*>(&x) };
	}
}

#include "kl/memory/bytes.msvc.hpp" // IWYU pragma: export
