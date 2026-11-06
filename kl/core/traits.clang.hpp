#pragma once

#include "kl/core/compiler.hpp"

#if KL_COMPILER_CLANG || KL_COMPILER_CLANG_CL

#include "kl/core/traits.hpp"

namespace kl
{
	template <typename T> using removeExtent     = __remove_extent(T);
	template <typename T> using removeAllExtents = __remove_all_extents(T);

	template <typename To, typename From> constexpr b8 isConvertibleTo         = __is_convertible_to(From, To);
	template <typename T>                 constexpr b8 isDestructible          = __is_destructible(T);
	template <typename T>                 constexpr b8 isTriviallyDestructible = __is_trivially_destructible(T);
}

#endif
