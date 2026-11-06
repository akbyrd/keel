#pragma once

#include "kl/core/compiler.hpp"

#if KL_COMPILER_MSVC

#include "kl/core/traits.hpp"

namespace kl
{
	template <typename T>        struct _removeExtent           { using Type = T; };
	template <typename T>        struct _removeExtent<T[]>      { using Type = T; };
	template <typename T, u64 N> struct _removeExtent<T[N]>     { using Type = T; };
	template <typename T>        struct _removeAllExtents       { using Type = T; };
	template <typename T>        struct _removeAllExtents<T[]>  { using Type = typename _removeAllExtents<T>::Type; };
	template <typename T, u64 N> struct _removeAllExtents<T[N]> { using Type = typename _removeAllExtents<T>::Type; };

	template <typename T> using removeExtent     = typename _removeExtent<T>::Type;
	template <typename T> using removeAllExtents = typename _removeAllExtents<T>::Type;

	template <typename To, typename From> constexpr b8 isConvertibleTo         = __is_convertible_to(From, To);
	template <typename T>                 constexpr b8 isDestructible          = __is_destructible(T);
	template <typename T>                 constexpr b8 isTriviallyDestructible = __is_trivially_destructible(T);
}

#endif
