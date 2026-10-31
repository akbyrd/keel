#pragma once

#if defined(_MSC_VER) && !defined(__clang__)

#include "kl/core/compiler.hpp"

#undef  KL_COMPILER_MSVC
#define KL_COMPILER_MSVC true

// Full signature of the enclosing function, including template arguments
#define KL_FUNCTION_SIGNATURE __FUNCSIG__

namespace kl
{
	constexpr Compiler compiler = Compiler::msvc;
}

#endif
