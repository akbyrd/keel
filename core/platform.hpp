#pragma once

#include "core/types.hpp"

namespace kl
{
	enum class Platform : u8
	{
		win32,
	};

	// TODO: Passed in by build system? (Generate it)

	//constexpr Platform platform;
}

#define KL_PLATFORM_WIN32 false

#include "core/platform.win32.hpp" // IWYU pragma: export

#if !defined(KL_PLATFORM_WIN32)
	#error "Unsupported platform"
#endif
