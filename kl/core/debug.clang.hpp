#pragma once

#include "kl/core/compiler.hpp"

#if KL_COMPILER_CLANG

#include "kl/core/debug.hpp"
#include "kl/core/traits.hpp"

namespace kl
{
	constexpr void
	assert(b8 condition)
	{
		if (!condition)
		{
			// NOTE: The compiler assumes you can never continue past __builtin_trap. This could cause
			// problems when debugging.

			__builtin_debugtrap();
			__builtin_trap();
		}
	}

	constexpr void
	debugBreak()
	{
		if (isConstantEvaluated())
		{
			// Do nothing
		}
		else
		{
			__builtin_debugtrap();
		}
	}
}

#endif
