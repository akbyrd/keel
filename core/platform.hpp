#pragma once

#include "core/types.hpp"

enum class Platform : u8
{
	win32,
};

// TODO: Passed in by build system? (Generate it)

#define BLD_PLATFORM_WIN32 false
//constexpr Platform platform;

#include "core/platform.win32.hpp" // IWYU pragma: export

#if !defined(BLD_PLATFORM_WIN32)
	#error "Unsupported platform"
#endif
