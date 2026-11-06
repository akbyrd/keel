#pragma once

#include "kl/core/types.hpp"

namespace kl
{
	enum class Compiler : u8
	{
		clang,
		clang_cl,
		gcc,
		msvc,
	};

	//constexpr Compiler compiler;
}

// TODO: Warning for undefined macros
// TODO: Passed in by build system?

#define KL_COMPILER_CLANG    false
#define KL_COMPILER_CLANG_CL false
#define KL_COMPILER_GCC      false
#define KL_COMPILER_MSVC     false

#include "kl/core/compiler.clang.hpp"    // IWYU pragma: export
#include "kl/core/compiler.clang-cl.hpp" // IWYU pragma: export
#include "kl/core/compiler.gcc.hpp"      // IWYU pragma: export
#include "kl/core/compiler.msvc.hpp"     // IWYU pragma: export

#if \
	!KL_COMPILER_CLANG && \
	!KL_COMPILER_CLANG_CL && \
	!KL_COMPILER_GCC && \
	!KL_COMPILER_MSVC
	#error "Unsupported compiler"
#endif
