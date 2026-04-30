#pragma once

#include "core/compiler.hpp"

#if BLD_COMPILER_MSVC

#include "core/traits.hpp"
#include "memory/bytes.hpp"

extern "C"
{
	void* __cdecl memcpy(void*, const void*, u64);
	void* __cdecl memmove(void*, const void*, u64);
	void* __cdecl memset(void*, i32, u64);
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
		memset(dst.data, 0, dst.len);
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
		memset(dst.data, src, dst.len);
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
		memcpy(dst.data, src.data, src.len);
	}
}

constexpr void
moveBytes(Span<u8> dst, Span<u8> src)
{
	assert(dst.len >= src.len);

	if (isConstantEvaluated())
	{
		if (dst.data < src.data)
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
		memmove(dst.data, src.data, src.len);
	}
}

#endif
