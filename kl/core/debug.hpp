#pragma once

#include "kl/core/types.hpp"

namespace kl
{
	constexpr void assert(b8 condition);
	constexpr void debugBreak();
}

#include "kl/core/debug.clang.hpp" // IWYU pragma: export
#include "kl/core/debug.msvc.hpp"  // IWYU pragma: export
