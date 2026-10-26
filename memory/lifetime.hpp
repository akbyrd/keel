#pragma once

#include "containers/span.hpp"
#include "core/traits.hpp"
#include "memory/bytes.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

namespace kl
{
	// TODO: Prevent move on const&
	template <typename T> constexpr removeRef<T>&& move(T&& x);
	template <typename T> constexpr MoveSpan<T> move(Span<T> x);
	//template <typename T> constexpr T&& forward(removeRef<T>& x);
	//template <typename T> constexpr T&& forward(removeRef<T>&& x);
	template <typename T> constexpr T&& forward(T&& x);

	// TODO: Think about the exact set of operations we want
	template <typename T, typename... Args> constexpr void construct(T& dst, Args&& ...args);
	template <typename T, typename... Args> constexpr void construct(Span<T> dst, Args&& ...args);
	template <typename T> constexpr void defaultConstruct(T& dst);
	template <typename T> constexpr void defaultConstruct(Span<T> dst);
	template <typename T> constexpr void valueConstruct(T& dst);
	template <typename T> constexpr void valueConstruct(Span<T> dst);
	template <typename T> constexpr void copyConstruct(T& dst, const T& src);
	template <typename T> constexpr void copyConstruct(Span<T> dst, const T& src);
	template <typename T> constexpr void copyConstruct(Span<T> dst, Span<const T> src);
	template <typename T> constexpr void moveConstruct(T& dst, T&& src);
	template <typename T> constexpr void moveConstruct(Span<T> dst, T&& src);
	template <typename T> constexpr void moveConstruct(Span<T> dst, MoveSpan<T> src);
	template <typename T> constexpr void copyAssign(T& dst, const T& src);
	template <typename T> constexpr void copyAssign(Span<T> dst, const T& src);
	template <typename T> constexpr void copyAssign(Span<T> dst, Span<const T> src);
	template <typename T> constexpr void moveAssign(T& dst, T&& src);
	template <typename T> constexpr void moveAssign(Span<T> dst, T&& src);
	template <typename T> constexpr void moveAssign(Span<T> dst, MoveSpan<T> src);
	template <typename T> constexpr void destruct(T& dst);
	template <typename T> constexpr void destruct(Span<T> dst);
	template <typename T> constexpr void relocate(T& dst, T&& src);
	template <typename T> constexpr void relocate(Span<T> dst, MoveSpan<T> src);

	struct PlacementTag {};
}

inline void* operator new   (kl::u64, void* x, kl::PlacementTag) { return x; }
inline void  operator delete(void*, void*, kl::PlacementTag) {}

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
	constexpr MoveSpan<T>
	move(Span<T> x)
	{
		return x;
	}

	#if false
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
	#else
	// TODO: Test this thoroughly
	template <typename T>
	constexpr T&&
	forward(T&& x)
	{
		if constexpr (isRValueRef<decltype(x)>)
			static_assert(!isLValueRef<T>);

		return static_cast<T&&>(x);
	}
	#endif

	template <typename T, typename... Args>
	constexpr void
	construct(T& dst, Args&& ...args)
	{
		new (&dst, PlacementTag{}) T { forward(args)... };
	}

	template <typename T, typename... Args>
	constexpr void
	construct(Span<T> dst, Args&& ...args)
	{
		new (&dst.data[0], PlacementTag{}) T { forward(args)... };
		for (i64 i = 1; i < dst.len; ++i)
			dst.data[i] = dst.data[0];
	}

	template <typename T>
	constexpr void
	defaultConstruct(T& dst)
	{
		new (&dst, PlacementTag{}) T;
	}

	template <typename T>
	constexpr void
	defaultConstruct(Span<T> dst)
	{
		new (dst.data, PlacementTag{}) T[dst.len];
	}

	template <typename T>
	constexpr void
	valueConstruct(T& dst)
	{
		new (&dst, PlacementTag{}) T {};
	}

	template <typename T>
	constexpr void
	valueConstruct(Span<T> dst)
	{
		new (dst.data, PlacementTag{}) T[dst.len] {};
	}

	template <typename T>
	constexpr void
	copyConstruct(T& dst, const T& src)
	{
		assert(&dst != src);
		new (&dst, PlacementTag{}) T { src };
	}

	template <typename T>
	constexpr void
	copyConstruct(Span<T> dst, const T& src)
	{
		assert(!dst.overlaps(src));
		new (dst.data, PlacementTag{}) T[dst.len] { src };
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
				new (&dst.data[i], PlacementTag{}) T { src.data[i] };
		}
	}

	template <typename T>
	constexpr void
	moveConstruct(T& dst, T&& src)
	{
		assert(&dst != &src);

		// TODO: move or forward?
		new (&dst, PlacementTag{}) T { move(src) };
	}

	template <typename T>
	constexpr void
	moveConstruct(Span<T> dst, T&& src)
	{
		assert(!dst.overlaps(src));

		// TODO: move or forward?
		new (dst.data, PlacementTag{}) T { move(src) };
		if (dst.len > 1)
			new (&dst.data[1], PlacementTag{}) T[dst.len - 1] { dst.data[0] };
	}

	// TODO: Is this correct? (check all type traits in this file)
	// TODO: Implement proper Span length checks for everything in this file
	// * Are operations driven by the length of src or dst? Do they have to match?
	// * What happens with zero length Spans?
	template <typename T>
	constexpr void
	moveConstruct(Span<T> dst, MoveSpan<T> src)
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
				new (&dst.data[i], PlacementTag{}) T { move(src.data[i]) };
		}
	}

	template <typename T>
	constexpr void
	copyAssign(T& dst, const T& src)
	{
		// TODO: Use isTriviallyCopyAssignable
		dst = src;
	}

	template <typename T>
	constexpr void
	copyAssign(Span<T> dst, const T& src)
	{
		// TODO: Use isTriviallyCopyAssignable
		// TODO: Maybe want something like spreadBytes?
		for (i64 i = 0; i < dst.len; ++i)
			dst.data[i] = src;
	}

	// TODO: Is this correct? (check all type traits in this file)
	template <typename T>
	constexpr void
	copyAssign(Span<T> dst, Span<const T> src)
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

	template <typename T>
	constexpr void
	moveAssign(T& dst, T&& src)
	{
		// TODO: Use isTriviallyMoveAssignable
		// TODO: move or forward?
		dst = move(src);
	}

	template <typename T>
	constexpr void
	moveAssign(Span<T> dst, T&& src)
	{
		// TODO: Use isTriviallyMoveAssignable

		assert(dst.len >= src.len);
		assert(!dst.overlaps(src));

		// TODO: move or forward?
		dst[0] = move(src);
		for (i64 i = 1; i < src.len; ++i)
			dst.data[i] = dst.data[0];
	}

	// TODO: Is this correct? (check all type traits in this file)
	template <typename T>
	constexpr void
	moveAssign(Span<T> dst, MoveSpan<T> src)
	{
		if constexpr (isTriviallyMoveAssignable<T>)
		{
			copyBytes(dst.asBytes(), src.asBytes());
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
	destruct(T& dst)
	{
		if constexpr (isTriviallyDestructible<T>)
		{
			// Nothing to do
		}
		else
		{
			dst.~T();
		}
	}

	template <typename T>
	constexpr void
	relocate(T& dst, T&& src)
	{
		assert(&dst != &src);

		dst = move(src);
		destruct(src);
	}

	template <typename T>
	constexpr void
	relocate(Span<T> dst, MoveSpan<T> src)
	{
		assert(!dst.overlaps(src));

		for (i64 i = 0; i < src.len; ++i)
		{
			dst.data[i] = move(src.data[i]);
			destruct(src.data[i]);
		}
	}
}
