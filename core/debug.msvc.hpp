#pragma once

#include "core/compiler.hpp"

#if KL_COMPILER_MSVC || KL_COMPILER_CLANG_CL

#include "core/debug.hpp"
#include "core/traits.hpp"

namespace kl
{
	constexpr void assert(b8 condition)
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

	constexpr void assertWin32(b8 condition)
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

	constexpr void debugBreak()
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
