#pragma once

#include "kl/core/types.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

namespace kl
{
	constexpr void assert(b8 condition);
	constexpr void debugBreak();
}

#include "kl/debug/assert.clang.hpp" // IWYU pragma: export
#include "kl/debug/assert.gcc.hpp"   // IWYU pragma: export
#include "kl/debug/assert.msvc.hpp"  // IWYU pragma: export

// -------------------------------------------------------------------------------------------------
// Implementation

namespace kl
{
	// NOTE: This suppresses warning C4800 - implicit conversion to bool
	template <typename T>
	constexpr void
	assert(const T& condition)
	{
		assert(b8(condition));
	}
}
