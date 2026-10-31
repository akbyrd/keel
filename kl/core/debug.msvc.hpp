#pragma once

#include "kl/core/compiler.hpp"

#if KL_COMPILER_MSVC || KL_COMPILER_CLANG_CL

#include "kl/core/debug.hpp"
#include "kl/core/traits.hpp"

namespace kl
{
	constexpr void
	assert(b8 condition)
	{
		if (!condition)
		{
			if (isConstantEvaluated())
			{
				throw false;
			}
			else
			{
				__debugbreak();
			}
		}
	}

	constexpr void
	assertWin32(b8 condition)
	{
		// TODO: Use error
		//u32 error = GetLastError();
		if (!condition)
		{
			if (isConstantEvaluated())
			{
				throw false;
			}
			else
			{
				__debugbreak();
			}
		}
	}

	template <typename T>
	constexpr void
	assertWin32(const T& condition)
	{
		assertWin32(b8(condition));
	}

	constexpr void
	debugBreak()
	{
		if (isConstantEvaluated())
		{
		}
		else
		{
			__debugbreak();
		}
	}
}

#endif
