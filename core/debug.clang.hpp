#pragma once

#include "core/compiler.hpp"

#if BLD_COMPILER_CLANG

#include "core/debug.hpp"

constexpr void assert(b8 condition)
{
	if (!condition)
	{
		__builtin_debugtrap();
		__builtin_trap();
	}
}

constexpr void debugBreak()
{
	__builtin_debugtrap();
}

#endif
