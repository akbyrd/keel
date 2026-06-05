/// @file
/// Detects and exposes the current compiler. Two mechanisms are provided for different contexts: a
/// constexpr variable and a set of macros. The constexpr variable is preferred whenever possible.
/// Macros are always defined as either true or false, never undefined. An unsupported compiler
/// triggers a compile error.
///
/// 1. constexpr variable (preferred)
/// Use for static branching:
/// ```cpp
/// if constexpr (compiler == Compiler::msvc)
/// ```
///
/// 2. macros
/// Use for conditional compilation:
/// ```cpp
/// #if BLD_COMPILER_MSVC
/// __msvc_only_intrinsic()
/// #endif
/// ```

#pragma once

#include "core/types.hpp"

/// The set of supported compilers.
enum class Compiler : u8
{
	clang,    ///< Compiling with clang (not clang-cl).
	clang_cl, ///< Compiling with clang-cl (not clang).
	msvc,     ///< Compiling with msvc.
};

/// The compiler that is currently being used. Declared and defined in the relevant
/// compiler-specific file since constexpr variables can't be forward declared.
/*constexpr Compiler compiler;*/

#define BLD_COMPILER_CLANG    false ///< @copydoc Compiler::clang
#define BLD_COMPILER_CLANG_CL false ///< @copydoc Compiler::clang_cl
#define BLD_COMPILER_MSVC     false ///< @copydoc Compiler::msvc

#include "core/compiler.clang.hpp"    // IWYU pragma: export
#include "core/compiler.clang-cl.hpp" // IWYU pragma: export
#include "core/compiler.msvc.hpp"     // IWYU pragma: export

#if \
	!BLD_COMPILER_CLANG && \
	!BLD_COMPILER_CLANG_CL && \
	!BLD_COMPILER_MSVC
	#error "Unsupported compiler"
#endif
