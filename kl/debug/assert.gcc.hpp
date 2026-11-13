#pragma once

#include "kl/core/compiler.hpp"

#if KL_COMPILER_GCC

#include "kl/core/traits.hpp"
#include "kl/debug/assert.hpp"

namespace kl
{
	constexpr void
	assert(b8 condition)
	{
		if (!condition)
		{
			// NOTE: The compiler assumes you can never continue past __builtin_trap. This could cause
			// problems when debugging.

			if (isConstantEvaluated())
			{
				__builtin_trap();
			}
			else
			{
				asm ("int3; nop");
				__builtin_trap();
			}
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
			asm ("int3; nop");
		}
	}
}

#endif
