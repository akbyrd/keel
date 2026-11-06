#pragma once

#if defined(__linux__)

#include "kl/core/platform.hpp"

#undef  KL_PLATFORM_LINUX
#define KL_PLATFORM_LINUX true

namespace kl
{
	constexpr Platform platform = Platform::linux;
}

#endif
