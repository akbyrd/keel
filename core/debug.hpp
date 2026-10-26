#pragma once

#include "core/types.hpp"

namespace kl
{

constexpr void assert(b8 condition);
constexpr void debugBreak();

}

#include "core/debug.clang.hpp" // IWYU pragma: export
#include "core/debug.msvc.hpp"  // IWYU pragma: export
