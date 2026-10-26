#pragma once

#if defined(_MSC_VER) && !defined(__clang__)

#include "core/compiler.hpp"

#undef  KL_COMPILER_MSVC
#define KL_COMPILER_MSVC true

namespace kl
{
	constexpr Compiler compiler = Compiler::msvc;
}

#endif
