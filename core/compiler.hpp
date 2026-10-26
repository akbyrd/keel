#pragma once

#include "core/types.hpp"

namespace kl
{

enum class Compiler : u8
{
	clang,
	clang_cl,
	msvc,
};

//constexpr Compiler compiler;

}

// TODO: Warning for undefined macros
// TODO: Passed in by build system?

#define KL_COMPILER_CLANG    false
#define KL_COMPILER_CLANG_CL false
#define KL_COMPILER_MSVC     false

#include "core/compiler.clang.hpp"    // IWYU pragma: export
#include "core/compiler.clang-cl.hpp" // IWYU pragma: export
#include "core/compiler.msvc.hpp"     // IWYU pragma: export

#if \
	!defined(KL_COMPILER_CLANG) && \
	!defined(KL_COMPILER_CLANG_CL) && \
	!defined(KL_COMPILER_MSVC)
	#error "Unsupported compiler"
#endif
