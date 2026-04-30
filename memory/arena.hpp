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

	Span<u8> bytes;
	i64      used;
	i64      reserved;
	Meta*    lastMeta;

	Arena() = default;
	~Arena();

	Arena(Span<const c8> name, Span<u8> bytes);
	Arena(Span<const c8> name, Allocation alloc, i64 reserved);

	Arena(const Arena&) = delete;
	Arena& operator=(const Arena&) = delete;

	Arena(Arena&&);
	Arena& operator=(Arena&&);

	template <typename T>                   constexpr T&      push      ();
	template <typename T>                   constexpr T&      push      (NoInit);
	template <typename T>                   constexpr T&      push      (T&& value);
	template <typename T, typename ...Args> constexpr T&      push      (Args&& ...args);

	//template <typename T>                   constexpr Span<T> pushArray (i64 count);
	//template <typename T>                   constexpr Span<T> pushArray (i64 count, NoInit);
	//template <typename T>                   constexpr Span<T> pushArray (i64 count, T&& value);
	//template <typename T, typename ...Args> constexpr Span<T> pushArray (i64 count, Args&& ...args);
	//template <typename T, typename ...Args> constexpr Span<T> pushArray (i64 count, Span<const T> value);

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

Arena::Arena(Span<const c8> name, Span<u8> bytes)
	: Arena(name, Allocation { bytes }, 0)
{}

Arena::Arena(Span<const c8> name, Allocation alloc, i64 reserved)
	: bytes(alloc.bytes)
	, used(0)
	, reserved(reserved)
	, lastMeta(nullptr)
{
	(void) name;

	// TODO: Implement
	//Span<c8> nameCopy = pushArray<c8>(name.len);
	//copyConstruct(nameCopy, name);
}

Arena::~Arena()
{
	for (; lastMeta; lastMeta = lastMeta->prevMeta)
	{
		const Meta& meta = *lastMeta;

		// TODO: Does the order of destruction matter?
		Span<u8> objects = { meta.count * meta.size, bytes.data + meta.offset };
		meta.destruct(objects);
	}
}

Arena::Arena(Arena&& x)
	: bytes(x.bytes)
	, used(x.used)
	, reserved(x.reserved)
	, lastMeta(x.lastMeta)
{
	x = {};
}

Arena& Arena::operator=(Arena&& x)
{
	bytes    = x.bytes;
	used     = x.used;
	reserved = x.reserved;
	lastMeta = x.lastMeta;

	x = {};
	return *this;
}

template <typename T>
constexpr T&
Arena::push()
{
	T& x = push(NoInit {});
	defaultConstruct(x);
}

template <typename T>
constexpr T&
Arena::push(NoInit)
{
	if constexpr (!isTriviallyDestructible<T>)
	{
		static_assert(TypeInfo<T>::align <= sizeof(Meta::offset));

		if (!lastMeta || lastMeta->id != id<T>)
		{
			Meta& meta = push<Meta>(NoInit {});
			u8* dst = alignUp<T>(bytes.data + used);

			new (&meta) Meta {
				.id       = TypeInfo<T>::id,
				.size     = TypeInfo<T>::size,
				.offset   = dst - bytes.data, // TODO: RelPtr?
				.count    = 0,
				.prevMeta = lastMeta,
				.destruct = TypeInfo<T>::destruct,
			};

			lastMeta = &meta;
		}

		lastMeta->count++;
	}

	// TODO: Try to simplify this
	u8* dst = alignUp<T>(bytes.data + used);
	u8* end = dst + sizeof(T);
	i64 required = end - bytes.data;
	if (required > bytes.len)
	{
		i64 newCap = bytes.len * goldenRatio<f32>;
		assert(reserved >= required);
		newCap = min(newCap, reserved);
		Allocation alloc = { bytes };
		VirtualAllocator::commit(alloc, newCap);
	}

	T& x = *reinterpret_cast<T*>(dst);
	return x;
}

template <typename T>
constexpr T&
Arena::push(T&& value)
{
	T& x = push(NoInit {});
	new (&x) T { move(value) };
}

template <typename T, typename ...Args>
constexpr T&
Arena::push(Args&& ...args)
{
	T& x = push(NoInit {});
	new (&x) T { forward(args...) };
}
