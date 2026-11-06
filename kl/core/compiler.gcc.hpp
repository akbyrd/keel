#pragma once

#if defined(__GNUC__) && !defined(__clang__)

#include "kl/core/compiler.hpp"

#undef  KL_COMPILER_GCC
#define KL_COMPILER_GCC true

namespace kl
{
	constexpr Compiler compiler = Compiler::gcc;
}

#endif
