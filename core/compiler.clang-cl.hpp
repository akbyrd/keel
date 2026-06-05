#pragma once

#if defined(_MSC_VER) && defined(__clang__)

#include "core/compiler.hpp"

#undef  BLD_COMPILER_CLANG_CL
#define BLD_COMPILER_CLANG_CL true

// The compiler that is currently being used.
constexpr Compiler compiler = Compiler::clang_cl;

#endif
