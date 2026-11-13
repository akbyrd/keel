#pragma once

#include "kl/containers/string-view.hpp"
#include "kl/core/traits.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

namespace kl
{
	u32  beginUtf8Output();
	void endUtf8Output(u32 codePage);

	void print(StringView x);
	void print(CStringView x);
	void print(b8 x);
	void print(c8 x);
	void print(i64 x);
	void print(u64 x);
	void print(f64 x);
	void print(const void* x);

	template <i64 N> void print(const c8 (&x)[N]);
	template <typename T> requires (isArithmetic<T> || isEnum<T>) void print(T x);
	template <typename... Args> requires (sizeof...(Args) > 1) void print(const Args&... args);
}

// -------------------------------------------------------------------------------------------------
// Implementation

namespace kl
{
	template <i64 N>
	void
	print(const c8 (&x)[N])
	{
		CStringView view = x;
		print(view);
	}

	template <typename T>
		requires (isArithmetic<T> || isEnum<T>)
	void
	print(T x)
	{
		if constexpr (isSignedInteger<T> || isEnum<T>)
			print(i64(x));
		else if constexpr (isIntegral<T>)
			print(u64(x));
		else
			print(f64(x));
	}

	template <typename... Args>
		requires (sizeof...(Args) > 1)
	void
	print(const Args&... args)
	{
		(print(args), ...);
	}
}
