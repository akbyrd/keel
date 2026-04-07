#pragma once

#include "containers/span.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

// NOTE: Allocations may be larger than requested by up to a page.

constexpr u64 Kilobyte = 1024;
constexpr u64 Megabyte = 1024 * Kilobyte;
constexpr u64 Gigabyte = 1024 * Megabyte;
constexpr u64 Terabyte = 1024 * Gigabyte;

struct Allocation;

struct Allocator
{
	using AllocFn = Allocation  (i64 size, u8 align);
	using FreeFn  = Allocation& (Allocation& allocation);

	AllocFn* alloc = nullptr;
	FreeFn*  free  = nullptr;
};

struct Allocation
{
	Span<u8> bytes;

	// TODO: const variant?
	Allocator getAllocator();
};

// TODO: Decide on initialization strategy
struct VirtualAllocator
{
	static constinit Allocator interface;

	static b8          init   ();
	static Allocation  alloc  (i64 size, u8 align);
	static Allocation  alloc  (i64 reserve, i64 commit, u8 align);
	static Allocation& commit (Allocation& allocation, i64 size);
	static Allocation& free   (Allocation& allocation);
};

struct LeakCheckAllocator
{
	static constinit Allocator interface;

	static b8          init  ();
	static Allocation  alloc (i64 size, u8 align);
	static Allocation& free  (Allocation& allocation);
};

// -------------------------------------------------------------------------------------------------
// Implementation

// TODO: This will conflict with <new>
inline void* operator new(u64, void* x) { return x; }
