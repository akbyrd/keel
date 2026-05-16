#pragma once

#include "containers/span.hpp"
#include "memory/lifetime.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

// NOTE: Allocations may be larger than requested by up to a page.

constexpr u64 Kilobyte = 1024;
constexpr u64 Megabyte = 1024 * Kilobyte;
constexpr u64 Gigabyte = 1024 * Megabyte;
constexpr u64 Terabyte = 1024 * Gigabyte;

// TODO: Destructor?
struct Allocation
{
	Span<u8> bytes;

	Allocation() = default;
	explicit Allocation(Span<u8> bytes);
	Allocation(const Allocation&) = default;
	Allocation& operator=(const Allocation&) = default;
	Allocation(Allocation&&);
	Allocation& operator=(Allocation&&);
};

struct VirtualAllocator
{
	static b8         init        ();
	static Allocation allocate    (i64 size, u8 align);
	static Allocation reserve     (i64 reserve, i64 commit, u8 align);
	static void       commit      (Allocation& alloc, i64 size);
	static void       free        (Allocation& alloc);
	static i64        getReserved (const Allocation& alloc);
};

// -------------------------------------------------------------------------------------------------
// Implementation

// TODO: This will conflict with <new>
constexpr void* operator new(u64, void* x) { return x; }

static_assert(sizeof(Allocation) == sizeof(Allocation::bytes));
static_assert(isTriviallyDestructible<Allocation>);
static_assert(isTriviallyConstructible<Allocation>);

inline
Allocation::Allocation(Span<u8> bytes)
	: bytes(bytes)
{}

inline
Allocation::Allocation(Allocation&& x)
	: Allocation(x)
{
	valueConstruct(x);
}

inline Allocation&
Allocation::operator=(Allocation&& x)
{
	// TODO: Handle self move
	assert(this != &x);

	*this = x;
	valueConstruct(x);
	return *this;
}
