#pragma once

#include "containers/span.hpp"
#include "core/traits.hpp"
#include "memory/bytes.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

// TODO: Prevent move on const&
template <typename T> constexpr removeRef<T>&& move(T&& x);
template <typename T> constexpr T&& forward(removeRef<T>& x);
template <typename T> constexpr T&& forward(removeRef<T>&& x);

// TODO: Think about the exact set of operations we want
template <typename T> constexpr void defaultConstruct(Span<T> dst);
template <typename T> constexpr void valueConstruct(Span<T> dst);
template <typename T> constexpr void copyConstruct(Span<T> dst, const T& src);
template <typename T> constexpr void copyConstruct(Span<T> dst, Span<const T> src);
template <typename T> constexpr void moveConstruct(Span<T> dst, T&& src);
template <typename T> constexpr void moveConstruct(Span<T> dst, Span<T> src);
template <typename T> constexpr void destruct(Span<T> dst);
template <typename T> constexpr void relocate(Span<T> dst, Span<T> src);
template <typename T> constexpr void copyAssign(Span<T> dst, const T& src);
template <typename T> constexpr void copyAssign(Span<T> dst, Span<const T> src);
template <typename T> constexpr void moveAssign(Span<T> dst, T&& src);
template <typename T> constexpr void moveAssign(Span<T> dst, Span<T> src);

// -------------------------------------------------------------------------------------------------
// Implementation

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
	static_assert(!isLValueRef<T>);
	return static_cast<T&&>(x);
}

// TODO: Is this correct? (check all type traits in this file)
template <typename T>
constexpr void copyAssign(Span<T> dst, Span<const T> src)
{
	if constexpr (isTriviallyCopyAssignable<T>)
	{
		copyBytes(dst.asBytes(), src.asBytes());
	}
	else
	{
		assert(dst.len >= src.len);
		assert(!dst.overlaps(src));

		for (i64 i = 0; i < src.len; ++i)
			dst.data[i] = src.data[i];
	}
}

// TODO: Is this correct? (check all type traits in this file)
template <typename T>
constexpr void moveAssign(Span<T> dst, Span<T> src)
{
	if constexpr (isTriviallyMoveAssignable<T>)
	{
		moveBytes(dst.asBytes(), src.asBytes());
	}
	else
	{
		assert(dst.len >= src.len);
		assert(!dst.overlaps(src));

		for (i64 i = 0; i < src.len; ++i)
			// TODO: move or forward?
			dst.data[i] = move(src.data[i]);
	}
}

template <typename T>
constexpr void
defaultConstruct(Span<T> dst)
{
	new (dst.data) T[dst.len];
}

template <typename T>
constexpr void
valueConstruct(Span<T> dst)
{
	new (dst.data) T[dst.len] {};
}

template <typename T>
constexpr void
copyConstruct(Span<T> dst, const T& src)
{
	assert(!dst.overlaps(src));
	new (dst.data) T[dst.len] { src };
}

// TODO: Is this correct? (check all type traits in this file)
template <typename T>
constexpr void
copyConstruct(Span<T> dst, Span<const T> src)
{
	if constexpr (isTriviallyCopyConstructible<T>)
	{
		copyBytes(dst.asBytes(), src.asBytes());
	}
	else
	{
		assert(!dst.overlaps(src));

		for (i64 i = 0; i < src.len; ++i)
			new (&dst.data[i]) T { src.data[i] };
	}
}

template <typename T>
constexpr void
moveConstruct(Span<T> dst, T&& src)
{
	assert(!dst.overlaps(src));

	// TODO: move or forward?
	new (dst.data) T { move(src) };
	if (dst.len > 1)
		new (&dst.data[1]) T[dst.len - 1] { dst.data[0] };
}

// TODO: Is this correct? (check all type traits in this file)
template <typename T>
constexpr void
moveConstruct(Span<T> dst, Span<T> src)
{
	if constexpr (isTriviallyMoveConstructible<T>)
	{
		copyBytes(dst.asBytes(), src.asBytes());
	}
	else
	{
		assert(!dst.overlaps(src));

		for (i64 i = 0; i < src.len; ++i)
			// TODO: move or forward?
			new (&dst.data[i]) T { move(src.data[i]) };
	}
}

template <typename T>
constexpr void
destruct(Span<T> dst)
{
	if constexpr (isTriviallyDestructible<T>)
	{
		// Nothing to do
	}
	else
	{
		for (T& x : dst)
			x.~T();
	}
}

template <typename T>
constexpr void
relocate(Span<T> dst, Span<T> src)
{
	assert(!dst.overlaps(src));

	for (i64 i = 0; i < src.len; ++i)
	{
		dst.data[i] = move(src.data[i]);
		destruct(src.data[i]);
	}
}
