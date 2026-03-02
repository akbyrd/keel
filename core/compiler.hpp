#pragma once

#include "core/types.hpp"

enum class Compiler : u8
{
	clang,
	clang_cl,
	msvc,
};

// TODO: Warning for undefined macros
// TODO: Passed in by build system?

#define BLD_COMPILER_CLANG    false
#define BLD_COMPILER_CLANG_CL false
#define BLD_COMPILER_MSVC     false
//constexpr Compiler compiler;

#include "core/compiler.clang.hpp"    // IWYU pragma: export
#include "core/compiler.clang-cl.hpp" // IWYU pragma: export
#include "core/compiler.msvc.hpp"     // IWYU pragma: export

#if \
	!defined(BLD_COMPILER_CLANG) && \
	!defined(BLD_COMPILER_CLANG_CL) && \
	!defined(BLD_COMPILER_MSVC)
	#error "Unsupported compiler"
#endif
