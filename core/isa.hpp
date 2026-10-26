#pragma once

#include "core/types.hpp"

enum class ISA : u8
{
	x64,
};

#define KL_ISA_X64 false
//constexpr ISA isa;

#include "core/isa.x64.hpp" // IWYU pragma: export

#if !defined(KL_ISA_X64)
	#error "Unsupported instruction set"
#endif
