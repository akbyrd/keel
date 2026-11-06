#pragma once

#include "kl/core/types.hpp"

namespace kl
{
	enum class Platform : u8
	{
		win32,
		linux,
	};

	// TODO: Passed in by build system? (Generate it)

	//constexpr Platform platform;
}

#define KL_PLATFORM_WIN32 false
#define KL_PLATFORM_LINUX false

#include "kl/core/platform.win32.hpp" // IWYU pragma: export
#include "kl/core/platform.linux.hpp" // IWYU pragma: export

#if \
	!KL_PLATFORM_WIN32 && \
	!KL_PLATFORM_LINUX
	#error "Unsupported platform"
#endif
