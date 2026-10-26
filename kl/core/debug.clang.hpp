#pragma once

#include "kl/core/compiler.hpp"

#if KL_COMPILER_CLANG

#include "kl/core/debug.hpp"

namespace kl
{
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
}

#endif
