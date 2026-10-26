#pragma once

#if defined(__clang__) && !defined(_MSC_VER)

#include "core/compiler.hpp"

#undef  KL_COMPILER_CLANG
#define KL_COMPILER_CLANG true

namespace kl
{
	constexpr Compiler compiler = Compiler::clang;
}

#endif
