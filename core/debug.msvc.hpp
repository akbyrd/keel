#pragma once

#include "core/compiler.hpp"

#if BLD_COMPILER_MSVC || BLD_COMPILER_CLANG_CL

#include "core/debug.hpp"
#include "core/traits.hpp"

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

#endif
