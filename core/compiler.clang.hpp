#pragma once

#if defined(__clang__) && !defined(_MSC_VER)

#include "core/compiler.hpp"

#undef  BLD_COMPILER_CLANG
#define BLD_COMPILER_CLANG true

constexpr Compiler compiler = Compiler::clang;

#endif
