#pragma once

#if defined(__clang__) && !defined(_MSC_VER)

#include "kl/core/compiler.hpp"

#undef  KL_COMPILER_CLANG
#define KL_COMPILER_CLANG true

// Full signature of the enclosing function, including template arguments
#define KL_FUNCTION_SIGNATURE __PRETTY_FUNCTION__

namespace kl
{
	constexpr Compiler compiler = Compiler::clang;
}

#endif
