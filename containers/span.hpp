#pragma once

#include "core/debug.hpp"
#include "core/traits.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

namespace kl
{
	// TODO: Can we separate the concepts of ownership and containers?
	// TODO: Consider renaming to View
	// More versatile - string view vs string span
	// More intuitive - string view vs string span

	template <typename T>
	struct Span
	{
		i64 len;
		T*  data;

		Span() = default;
		constexpr Span(T& x);
		constexpr Span(i64 len, T* data);
		template <i64 N> constexpr Span(T (&x)[N]);

		constexpr T&       operator[] (i64 x);
		constexpr const T& operator[] (i64 x) const;

		constexpr operator Span<const T>() const;
		constexpr explicit operator bool() const;

		constexpr T*       begin ();
		constexpr const T* begin () const;
		constexpr T*       end   ();
		constexpr const T* end   () const;

		//constexpr T&       first   ();
		//constexpr const T& first   () const;
		//constexpr T&       firstOr (T& x);
		//constexpr const T& firstOr (T&&) const = delete;
		//constexpr const T& firstOr (const T& x) const;
		//constexpr T&       last    ();
		//constexpr const T& last    () const;
		//constexpr T&       lastOr  (T& x);
		//constexpr const T& lastOr  (T&&) const = delete;
		//constexpr const T& lastOr  (const T& x) const;

		constexpr Span<T> resize      (i64 frontDelta, i64 backDelta) const;
		constexpr Span<T> resizeFront (i64 frontDelta) const;
		constexpr Span<T> resizeBack  (i64 backDelta) const;

		constexpr Span<T> next (i64 len) const;
		constexpr Span<T> prev (i64 len) const;

		constexpr bool overlaps(Span<const T> x) const;

		constexpr Span<u8> asBytes() requires (!isConst<T>);
		constexpr Span<const u8> asBytes() const;

		template <typename U>
			requires (
				isSame<removeC<T>, u8> &&
				(isConst<U> || !isConst<T>)
			)
		constexpr Span<U> as() const;
	};

	template <typename T>
	struct MoveSpan : Span<T>
	{
		constexpr T&&      operator[] (i64 x);
		constexpr const T& operator[] (i64 x) const;
	};
}

// -------------------------------------------------------------------------------------------------
// Implementation

namespace kl
{
	//template <typename T>
	//Span(const T&) -> Span<const T>;

	//template <typename T>
	//Span(const T (&)[]) -> Span<const T>;

	template <typename T>
	constexpr
	Span<T>::Span(T& x)
		: len(1)
		, data(&x)
	{}

	template <typename T>
	constexpr
	Span<T>::Span(i64 len, T* data)
		: len(len)
		, data(data)
	{}

	template <typename T>
	template <i64 N>
	constexpr
	Span<T>::Span(T (&x)[N])
		: len(N)
		, data(x)
	{}

	template <typename T>
	constexpr T&
	Span<T>::operator[](i64 x)
	{
		assert(x < len);
		return data[x];
	}

	template <typename T>
	constexpr const T&
	Span<T>::operator[](i64 x) const
	{
		assert(x < len);
		return data[x];
	}

	template <typename T>
	constexpr
	Span<T>::operator Span<const T>() const
	{
		return { len, data };
	}

	template <typename T>
	constexpr Span<T>::operator bool() const
	{
		return len;
	}

	template <typename T>
	constexpr T*
	Span<T>::begin()
	{
		return data;
	}

	template <typename T>
	constexpr T*
	Span<T>::end()
	{
		return &data[len];
	}

	template <typename T>
	constexpr const T*
	Span<T>::begin() const
	{
		return data;
	}

	template <typename T>
	constexpr const T*
	Span<T>::end() const
	{
		return data[len];
	}

	#if false
	template <typename T>
	constexpr T&
	Span<T>::first()
	{
		assert(len > 0);
		return data[0];
	}

	template <typename T>
	constexpr const T&
	Span<T>::first() const
	{
		assert(len > 0);
		return data[0];
	}

	template <typename T>
	constexpr T&
	Span<T>::firstOr(T& x)
	{
		return len > 0 ? data[0] : x;
	}

	template <typename T>
	constexpr const T&
	Span<T>::firstOr (const T& x) const
	{
		return len > 0 ? data[0] : x;
	}

	template <typename T>
	constexpr T&
	Span<T>::last()
	{
		assert(len > 0);
		return data[len - 1];
	}

	template <typename T>
	constexpr const T&
	Span<T>::last() const
	{
		assert(len > 0);
		return data[len - 1];
	}

	template <typename T>
	constexpr T&
	Span<T>::lastOr(T& x)
	{
		return len > 0 ? data[len - 1] : x;
	}

	template <typename T>
	constexpr const T&
	Span<T>::lastOr(const T& x) const
	{
		return len > 0 ? data[len - 1] : x;
	}
	#endif

	template <typename T>
	constexpr Span<T>
	Span<T>::resize(i64 frontDelta, i64 backDelta) const
	{
		return { len - frontDelta + backDelta, data + frontDelta };
	}

	template <typename T>
	constexpr Span<T>
	Span<T>::resizeFront(i64 frontDelta) const
	{
		return { len - frontDelta, data + frontDelta };
	}

	template <typename T>
	constexpr Span<T>
	Span<T>::resizeBack(i64 backDelta) const
	{
		return { len + backDelta, data };
	}

	template <typename T>
	constexpr Span<T> Span<T>::next(i64 len) const
	{
		return { len, data + this->len };
	}

	template <typename T>
	constexpr Span<T> Span<T>::prev(i64 len) const
	{
		return { len, data - len };
	}

	template <typename T>
	constexpr bool
	Span<T>::overlaps(Span<const T> x) const
	{
		return data >= x.data + x.len || x.data >= data + len;
	}

	template <typename T>
	constexpr Span<u8>
	Span<T>::asBytes()
		requires (!isConst<T>)
	{
		return { len * i64(sizeof(T)), reinterpret_cast<u8*>(data) };
	}

	template <typename T>
	constexpr Span<const u8>
	Span<T>::asBytes() const
	{
		return { len * i64(sizeof(T)), reinterpret_cast<const u8*>(data) };
	}

	template <typename T>
	template <typename U>
		requires (
			isSame<removeC<T>, u8> &&
			(isConst<U> || !isConst<T>)
		)
	constexpr Span<U>
	Span<T>::as() const
	{
		return { len / sizeof(U), reinterpret_cast<U*>(data) };
	}

	template <typename T>
	constexpr T&&
	MoveSpan<T>::operator[] (i64 x)
	{
		using Base = Span<T>;
		assert(x < Base::len);
		return move(Base::data[x]);
	}
}
