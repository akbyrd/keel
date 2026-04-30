#pragma once

#include "containers/span.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

// NOTE: Allocations may be larger than requested by up to a page.

constexpr u64 Kilobyte = 1024;
constexpr u64 Megabyte = 1024 * Kilobyte;
constexpr u64 Gigabyte = 1024 * Megabyte;
constexpr u64 Terabyte = 1024 * Gigabyte;

struct Allocation
{
	Span<u8> bytes;
	i64      reserved;
};

struct VirtualAllocator
{
	static b8          init     ();
	static Allocation  allocate (i64 size, u8 align);
	static Allocation  reserve  (i64 reserve, i64 commit, u8 align);
	static Allocation& commit   (Allocation& alloc, i64 size);
	static Allocation& free     (Allocation& alloc);
};

// -------------------------------------------------------------------------------------------------
// Implementation

// TODO: This will conflict with <new>
inline void* operator new(u64, void* x) { return x; }
