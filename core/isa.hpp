#pragma once

#include "core/types.hpp"

namespace kl
{
	enum class ISA : u8
	{
		x64,
	};

	//constexpr ISA isa;
}

#define KL_ISA_X64 false

#include "core/isa.x64.hpp" // IWYU pragma: export

#if !defined(KL_ISA_X64)
	#error "Unsupported instruction set"
#endif
