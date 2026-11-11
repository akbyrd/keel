#pragma once

#include "kl/containers/span.hpp"
#include "kl/core/traits.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

namespace kl
{
	template <typename T> requires (!isConst<T>) constexpr Span<u8>       toBytes(T& x);
	template <typename T>                        constexpr Span<const u8> toBytes(const T& x);

	constexpr void clearBytes (Span<u8> dst);
	constexpr void fillBytes  (Span<u8> dst, u8 src);
	constexpr void copyBytes  (Span<u8> dst, Span<const u8> src);
	constexpr void moveBytes  (Span<u8> dst, Span<const u8> src);
}

// -------------------------------------------------------------------------------------------------
// Implementation

namespace kl
{
	extern "C"
	{
		void* memcpy(void*, const void*, ux);
		void* memmove(void*, const void*, ux);
		void* memset(void*, i32, ux);
	}

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

	constexpr void
	clearBytes(Span<u8> dst)
	{
		if (isConstantEvaluated())
		{
			for (u8& x : dst)
				x = 0;
		}
		else
		{
			assert(dst.data);
			memset(dst.data, 0, ux(dst.len));
		}
	}

	constexpr void
	fillBytes(Span<u8> dst, u8 src)
	{
		if (isConstantEvaluated())
		{
			for (u8& x : dst)
				x = src;
		}
		else
		{
			assert(dst.data);
			memset(dst.data, src, ux(dst.len));
		}
	}

	constexpr void
	copyBytes(Span<u8> dst, Span<const u8> src)
	{
		assert(dst.len >= src.len);
		assert(!dst.overlaps(src));

		if (isConstantEvaluated())
		{
			for (i64 i = 0; i < src.len; ++i)
				dst.data[i] = src.data[i];
		}
		else
		{
			assert(dst.data);
			assert(src.data);
			memcpy(dst.data, src.data, ux(src.len));
		}
	}

	constexpr void
	moveBytes(Span<u8> dst, Span<const u8> src)
	{
		assert(dst.len >= src.len);

		if (isConstantEvaluated())
		{
			if (!dst.overlaps(src) || dst.data < src.data)
			{
				for (i64 i = 0; i < src.len; ++i)
					dst.data[i] = src.data[i];
			}
			else
			{
				for (i64 i = src.len - 1; i >= 0; --i)
					dst.data[i] = src.data[i];
			}
		}
		else
		{
			assert(dst.data);
			assert(src.data);
			memmove(dst.data, src.data, ux(src.len));
		}
	}
}
