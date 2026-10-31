#pragma once

#include "kl/core/traits.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

namespace kl
{
	// TODO: Prevent move on const&
	template <typename T> constexpr removeRef<T>&& move(T&& x);
	template <typename T> constexpr T&& forward(removeRef<T>& x);
	template <typename T> constexpr T&& forward(removeRef<T>&& x);
}

// -------------------------------------------------------------------------------------------------
// Implementation

namespace kl
{
	template <typename T>
	constexpr removeRef<T>&&
	move(T&& x)
	{
		return static_cast<removeRef<T>&&>(x);
	}

	template <typename T>
	constexpr T&&
	forward(removeRef<T>& x)
	{
		return static_cast<T&&>(x);
	}

	template <typename T>
	constexpr T&&
	forward(removeRef<T>&& x)
	{
		static_assert(!isLRef<T>);
		return static_cast<T&&>(x);
	}
}
