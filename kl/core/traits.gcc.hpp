#pragma once

#include "kl/core/compiler.hpp"

#if KL_COMPILER_GCC

#include "kl/core/traits.hpp"

namespace kl
{
	template <typename T> using removeExtent     = __remove_extent(T);
	template <typename T> using removeAllExtents = __remove_all_extents(T);

	template <typename To, typename From> constexpr b8 isConvertibleTo         = __is_convertible(From, To);
	template <typename T>                 constexpr b8 isDestructible          = isRef<T> || requires (T& t) { t.~T(); };
	template <typename T, u64 N>          constexpr b8 isDestructible<T[N]>    = isDestructible<T>;
	template <typename T>                 constexpr b8 isTriviallyDestructible = isDestructible<T> && __has_trivial_destructor(T);
}

#endif
