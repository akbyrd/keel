#pragma once

#include "core/meta.hpp"
#include "memory/allocator.hpp"
#include "memory/lifetime.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

// TODO: Is tag dispatch the best approach?
// TODO: Rename?
struct NoInit {};

// TODO: Design
// Don't replicate allocator interface
// Store allocator in Arena or allocation?

// TODO: Can we nest arenas?
// How do we ensure children end their lifetime before parents?
// Do arenas need to track their parent? (Debug only?)

// TODO: Maybe arrays are a special case of arenas?

#if false
struct Arena
{
	struct Meta;

	Span<u8> alloc    = {};
	Meta*    lastMeta = nullptr;

	Arena() = default; // TODO: Does this zero?
	~Arena();

	Arena(Span<const c8> name, i64 bytes, Allocator allocator);
	Arena(Span<const c8> name, i64 reserve, i64 commit, Allocator allocator);

	Arena(const Arena&) = delete;
	Arena& operator=(const Arena&) = delete;

	// TODO: Move is probably ok
	Arena(Arena&&) = delete;
	Arena& operator=(Arena&&) = delete;

	// TODO: I don't think any of this can be constexpr
	template <typename T>                   constexpr T&      push      ();
	template <typename T>                   constexpr T&      push      (NoInit);
	template <typename T>                   constexpr T&      push      (T&& value);
	template <typename T, typename ...Args> constexpr T&      push      (Args&& ...args);

	template <typename T>                   constexpr Span<T> pushArray (i64 count);
	template <typename T>                   constexpr Span<T> pushArray (i64 count, NoInit);
	template <typename T>                   constexpr Span<T> pushArray (i64 count, T&& value);
	template <typename T, typename ...Args> constexpr Span<T> pushArray (i64 count, Args&& ...args);

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

	using DestructFn = void(void*);
	DestructFn* destruct;
};

static_assert(isTriviallyConstructible<Arena::Meta>);
static_assert(isTriviallyDestructible<Arena::Meta>);
static_assert(isTriviallyCopyable<Arena::Meta>);

// TODO: Maybe name should be handed to the allocator?
Arena::Arena(Span<const c8> name, i64 bytes, Allocator allocator)
	: Arena(name, bytes, bytes, allocator)
{}

Arena::Arena(Span<const c8> name, i64 reserve, i64 commit, Allocator allocator)
	: alloc(allocator.alloc(reserve, commit))
{
	Span<c8> nameCopy = pushArray<c8>(name.len);
	copyConstruct(nameCopy, name);
}

Arena::~Arena()
{
	for (; lastMeta; lastMeta = lastMeta->prevMeta)
	{
		const Meta& meta = *lastMeta;

		u8* objects = alloc.bytes.data + meta.offset;
		for (i64 i = meta.count - 1; i >= 0; --i)
		{
			u8* object = objects + i * meta.size;
			meta.destruct(object);
		}
	}

	alloc.allocator.free(alloc.bytes);
}

template <typename T>
constexpr T&
Arena::push()
{
	T& x = push(NoInit {});
	defaultConstruct(x);
}

// TODO: There are a lot of dependencies on the allocator
// * Block allocator requires a list of blocks
// * Realloc requires a branch to skip relocation
// * Virtual allocator requires a branch to skip relocation
// * Overflow/underflow allocator requires custom offset on objects
// Should arenas be able to use different allocators?

// TODO: Who should make the initial allocation?
// Option 1 - The caller
//   Con: Asymmetric because the Arena is going to free it
// Option 2 - Arena
//   Con: Arena has to expose API for allocating
//     Changes to allocators affect arena init code

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
			u8* dst = alignUp<T>(alloc.bytes.data + alloc.bytes.len);

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

		lastMeta->count++;
	}

	u8* dst = alignUp<T>(alloc.bytes.data + alloc.bytes.len);
	u8* end = dst + sizeof(T);
	if (end > alloc.bytes.data + alloc.cap)
	{
		i64 newCap = alloc.cap * alloc.allocator.growthFactor;
		newCap = min(newCap, alloc.reserved);
		assert(newCap > alloc.cap);
		Span newBytes = alloc.bytes.next(newCap - alloc.cap);
		commitBytes(newBytes);
	}

	T& x = *static_cast<T*>(dst);
	return x;
}

template <typename T>
constexpr T&
Arena::push(T&& value)
{
	T& x = push(NoInit {});
	new (&x) T { forward(value) };
}

template <typename T, typename ...Args>
constexpr T&
Arena::push(Args&& ...args)
{
	T& x = push(NoInit {});
	new (&x) T { forward(args...) };
}
#endif
