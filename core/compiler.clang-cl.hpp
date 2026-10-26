#pragma once

#if defined(_MSC_VER) && defined(__clang__)

#include "core/compiler.hpp"

#undef  KL_COMPILER_CLANG_CL
#define KL_COMPILER_CLANG_CL true

namespace kl
{
	constexpr Compiler compiler = Compiler::clang_cl;
}

#endif
