#pragma once

#include "core/math.hpp"
#include "core/meta.hpp"
#include "memory/allocator.hpp"
#include "memory/lifetime.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

// TODO: Is tag dispatch the best approach?
// TODO: Rename?
struct NoInit {};

struct Arena
{
	struct Meta;

	Allocation alloc;
	i64        used;
	i64        reserved;
	Meta*      lastMeta;

	Arena() = default;
	~Arena();

	Arena(Span<const c8> name, Span<u8> bytes);
	Arena(Span<const c8> name, Allocation&& alloc);
	Arena(Span<const c8> name, i64 size, u8 align);
	Arena(Span<const c8> name, i64 reserve, i64 commit, u8 align);

	Arena(const Arena&) = delete;
	Arena& operator=(const Arena&) = delete;

	Arena(Arena&&);
	Arena& operator=(Arena&&);

	template <typename T>                   constexpr T&      push      ();
	template <typename T>                   constexpr T&      push      (NoInit);
	template <typename T>                   constexpr T&      push      (T&& value);
	template <typename T, typename ...Args> constexpr T&      push      (Args&& ...args);

	template <typename T>                   constexpr Span<T> pushArray (i64 count);
	template <typename T>                   constexpr Span<T> pushArray (i64 count, NoInit);
	template <typename T>                   constexpr Span<T> pushArray (i64 count, T&& value);
	template <typename T, typename ...Args> constexpr Span<T> pushArray (i64 count, Args&& ...args);
	template <typename T, typename ...Args> constexpr Span<T> pushArray (Span<const T> values);
	template <typename T, typename ...Args> constexpr Span<T> pushArray (MoveSpan<T> values);

	// TODO: scope
	// TODO: overhead (arena, allocator, total)
};

// -------------------------------------------------------------------------------------------------
// Implementation

// TODO: Pack
struct Arena::Meta
{
	i32   id;
	i64   size;
	i32   offset;
	i64   count;
	Meta* prevMeta;

	using DestructFn = void(Span<u8>);
	DestructFn* destruct;
};

inline
Arena::Arena(Span<const c8> name, Span<u8> bytes)
	: alloc(Allocation { bytes })
	, used(0)
	, reserved(0)
	, lastMeta(nullptr)
{
	pushArray<c8>(name);
}

inline
Arena::Arena(Span<const c8> name, Allocation&& alloc)
	: alloc(alloc)
	, used(0)
	, reserved(VirtualAllocator::getReserved(alloc))
	, lastMeta(nullptr)
{
	pushArray<c8>(name);
}

inline
Arena::Arena(Span<const c8> name, i64 size, u8 align)
	: alloc(VirtualAllocator::allocate(size, align))
	, used(0)
	, reserved(size)
	, lastMeta(nullptr)
{
	pushArray<c8>(name);
}

inline
Arena::Arena(Span<const c8> name, i64 reserve, i64 commit, u8 align)
	: alloc(VirtualAllocator::reserve(reserve, commit, align))
	, used(0)
	, reserved(reserve)
	, lastMeta(nullptr)
{
	pushArray<c8>(name);
}

inline
Arena::~Arena()
{
	for (; lastMeta; lastMeta = lastMeta->prevMeta)
	{
		const Meta& meta = *lastMeta;

		Span<u8> objects = { meta.count * meta.size, alloc.bytes.data + meta.offset };
		meta.destruct(objects);
	}

	if (reserved)
		VirtualAllocator::free(alloc);
}

inline
Arena::Arena(Arena&& x)
	: alloc(move(x.alloc))
	, lastMeta(x.lastMeta)
{
	x = {};
}

inline Arena&
Arena::operator=(Arena&& x)
{
	alloc = move(x.alloc);
	lastMeta = x.lastMeta;
	x.alloc = {};
	x.lastMeta = nullptr;
	return *this;
}

template <typename T>
constexpr T&
Arena::push()
{
	Span<T> x = pushArray<T>(1, NoInit {});
	defaultConstruct(x);
	return x[0];
}

template <typename T>
constexpr T&
Arena::push(NoInit)
{
	Span<T> x = pushArray<T>(1, NoInit {});
	return x[0];
}

template <typename T>
constexpr T&
Arena::push(T&& value)
{
	Span<T> x = pushArray<T>(1, NoInit {});
	moveConstruct(x, move(value));
	return x[0];
}

template <typename T, typename ...Args>
constexpr T&
Arena::push(Args&& ...args)
{
	Span<T> x = pushArray<T>(1, NoInit {});
	construct(x[0], forward(args)...);
	return x[0];
}

template <typename T>
constexpr Span<T>
Arena::pushArray(i64 count, NoInit)
{
	if constexpr (!isTriviallyDestructible<T>)
	{
		static_assert(TypeInfo<T>::align <= sizeof(Meta::offset));

		if (!lastMeta || lastMeta->id != id<T>)
		{
			Span<Meta> metas = pushArray<Meta>(1, NoInit {});
			u8* dst = alignUp<T>(alloc.bytes.data + used);

			Meta& meta = metas[0];
			// TODO: Compare
			//construct(meta, {
			new (&meta) Meta {
				.id       = TypeInfo<T>::id,
				.size     = TypeInfo<T>::size,
				.offset   = dst - alloc.bytes.data, // TODO: RelPtr?
				.count    = 0,
				.prevMeta = lastMeta,
				.destruct = TypeInfo<T>::destruct,
			};

			lastMeta = &meta;
		}

		lastMeta->count += count;
	}

	// TODO: Try to simplify this
	u8* dst = alignUp<T>(alloc.bytes.data + used);
	u8* end = dst + count * sizeof(T);
	i64 required = end - alloc.bytes.data;
	if (required > alloc.bytes.len)
	{
		if (reserved && required <= reserved)
		{
			i64 newCap = i64(alloc.bytes.len * goldenRatio<f32>);
			newCap = clamp(newCap, required, reserved);
			VirtualAllocator::commit(alloc, newCap);
		}
		else
		{
			// TODO: crash
			assert(false);
		}
	}

	Span<T> x = { count, reinterpret_cast<T*>(dst) };
	return x;
}

template <typename T>
constexpr Span<T>
Arena::pushArray(i64 count)
{
	Span<T> x = pushArray<T>(count, NoInit {});
	defaultConstruct(x);
}

template <typename T>
constexpr Span<T>
Arena::pushArray(i64 count, T&& value)
{
	Span<T> x = pushArray<T>(count, NoInit {});
	moveConstruct(x, forward(value));
}

template <typename T, typename ...Args>
constexpr Span<T>
pushArray (i64 count, Args&& ...args)
{
	Span<T> x = pushArray<T>(count, NoInit {});
	construct(x[0], forward(args)...);
	for (i64 i = 1; i < count; ++i)
		x[i] = x[0];
}

template <typename T, typename ...Args>
constexpr Span<T>
Arena::pushArray(Span<const T> values)
{
	Span<T> x = pushArray<T>(values.len, NoInit {});
	copyConstruct(x, values);
	return x;
}

template <typename T, typename ...Args>
constexpr Span<T>
Arena::pushArray(MoveSpan<T> values)
{
	Span<T> x = pushArray<T>(values.len, NoInit {});
	moveConstruct(x, values);
	return x;
}
