#pragma once

#include "core/compiler.hpp"

#if \
	(BLD_COMPILER_MSVC && defined(_M_X64)) || \
	(BLD_COMPILER_CLANG && defined(__x86_64__)) || \
	(BLD_COMPILER_CLANG_CL && defined(__x86_64__))

#include "core/isa.hpp"

#undef  BLD_ISA_X64
#define BLD_ISA_X64 true

constexpr ISA isa = ISA::x64;

#endif
