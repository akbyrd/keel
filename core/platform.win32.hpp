#pragma once

#if defined(_MSC_VER)

#include "core/platform.hpp"

#undef  KL_PLATFORM_WIN32
#define KL_PLATFORM_WIN32 true

namespace kl
{

constexpr Platform platform = Platform::win32;

}

#endif
