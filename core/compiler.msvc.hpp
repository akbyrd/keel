#pragma once

#if defined(_MSC_VER) && !defined(__clang__)

#include "core/compiler.hpp"

#undef  BLD_COMPILER_MSVC
#define BLD_COMPILER_MSVC true

// The compiler that is currently being used.
constexpr Compiler compiler = Compiler::msvc;

#endif
