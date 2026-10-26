#pragma once

#include "kl/core/compiler.hpp"

#if \
	(KL_COMPILER_MSVC && defined(_M_X64)) || \
	(KL_COMPILER_CLANG && defined(__x86_64__)) || \
	(KL_COMPILER_CLANG_CL && defined(__x86_64__))

#include "kl/core/isa.hpp"

#undef  KL_ISA_X64
#define KL_ISA_X64 true

namespace kl
{
	constexpr ISA isa = ISA::x64;
}

#endif
