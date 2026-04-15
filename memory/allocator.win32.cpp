#pragma once

#include "core/platform.hpp"

#if BLD_PLATFORM_WIN32

#include "core/math.hpp"
#include "core/win32.hpp"
#include "memory/allocator.hpp"

namespace AllocatorInternal
{
	static inline constexpr u16 id               = 0xA11C;
	static inline constexpr u8  maxAlign         = 64;
	static inline constinit u32 pageSize         = 0;
	static inline constinit u32 allocGranularity = 0;

	struct Header
	{
		// TODO: struct TypeId? struct AllocTypeIndex?
		// TODO: Rename
		u16 id;
		u8  headerOffset;
		u8  userOffset;
		i64 committed;
		i64 reserved;
	};

	static Header& getHeader(Allocation& allocation)
	{
		assert(allocation.bytes.data);
		i64 userAddr   = i64(allocation.bytes.data);
		i64 headerAddr = roundDown<i64>(userAddr - sizeof(Header), alignof(Header));

		Header& header = *reinterpret_cast<Header*>(headerAddr);
		assert(header.id == id);
		return header;
	}

	static LONG CALLBACK onGuardPageAccess(EXCEPTION_POINTERS* ep)
	{
		if (ep->ExceptionRecord->ExceptionCode != EXCEPTION_GUARD_PAGE)
			return EXCEPTION_CONTINUE_SEARCH;

		// TODO: Clean this up
		u8* faultAddr = reinterpret_cast<u8*>(ep->ExceptionRecord->ExceptionInformation[1]);
		faultAddr = (u8*) roundDown<i64>(i64(faultAddr), pageSize);

		u8** headerAddr = reinterpret_cast<u8**>(faultAddr);
		Header& header = *reinterpret_cast<Header*>(*headerAddr);
		assert(header.id == id);

		header.committed += pageSize;
		if (header.reserved > header.committed)
		{
			// TODO: Clean this up
			// NOTE: The guard page triggers onGuardPageAccess which commits it and advances the guard
			u8* guardBytes = static_cast<u8*>(faultAddr + pageSize);
			u8** headerPtr = reinterpret_cast<u8**>(guardBytes); // TODO: Rename
			void* guard = VirtualAlloc(guardBytes, pageSize, MEM_COMMIT, PAGE_READWRITE);
			assertWin32(guard);

			*headerPtr = *headerAddr;
			guard = VirtualAlloc(guardBytes, pageSize, MEM_COMMIT, PAGE_GUARD | PAGE_READWRITE);
			assertWin32(guard);
		}

		return EXCEPTION_CONTINUE_EXECUTION;
	}
}

b8
VirtualAllocator::init()
{
	using namespace AllocatorInternal;

	// TODO: Replace with isImplicitLifetime
	static_assert(isAggregate<Header>);
	static_assert(isTriviallyConstructible<Header>);
	static_assert(isTriviallyDestructible<Header>);
	static_assert(isPowerOf2(maxAlign));

	SYSTEM_INFO info;
	GetSystemInfo(&info);

	pageSize         = info.dwPageSize;
	allocGranularity = info.dwAllocationGranularity;
	assert(isMultipleOf(allocGranularity, pageSize));

	if (!isMultipleOf<u32>(pageSize, maxAlign))
	{
		// TODO: logging
		return false;
	}

	if (!isMultipleOf<u32>(allocGranularity, alignof(Header)))
	{
		// TODO: logging
		return false;
	}

	AddVectoredExceptionHandler(1, onGuardPageAccess);
	return true;
}

Allocation
VirtualAllocator::alloc(i64 size, u8 align)
{
	return alloc(size, size, align);
}

Allocation
VirtualAllocator::alloc(i64 reserve, i64 commit, u8 align)
{
	// Layout: [Slack | Header | Data]

	using namespace AllocatorInternal;

	assert(isPowerOf2(align));
	assert(align <= maxAlign);
	assert(reserve >= commit);
	assert(isMultipleOf<u32>(allocGranularity, align));

	i64 userOffset   = roundUp<i64>(sizeof(Header), align);
	i64 headerOffset = roundDown<i64>(userOffset - sizeof(Header), alignof(Header));
	assert(userOffset   <= maxVal<i64> - reserve);
	assert(userOffset   <  maxVal<u8>);
	assert(headerOffset <  maxVal<u8>);

	reserve = roundUp<i64>(userOffset + reserve, pageSize);
	commit  = roundUp<i64>(userOffset + commit,  pageSize);

	// NOTE: Memory is guaranteed to be zeroed
	// NOTE: When no address is specified size is rounded up to a multiple of page size

	u8* allocBytes = nullptr;
	u8* headerBytes = nullptr;
	Allocation allocation = {};

	// Case 1 - Commit all
	if (reserve == commit)
	{
		void* allocData = VirtualAlloc(nullptr, commit, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		assertWin32(allocData);

		allocBytes = static_cast<u8*>(allocData);
		headerBytes = allocBytes + headerOffset;
		allocation = Allocation { .bytes { commit - userOffset, allocBytes + userOffset } };
	}
	else
	{
		constexpr bool autoCommit = true;

		// Case 2 - Reserve + manual commit
		if constexpr (!autoCommit)
		{
			void* allocData = VirtualAlloc(nullptr, reserve, MEM_RESERVE, PAGE_NOACCESS);
			assertWin32(allocData);

			allocBytes = static_cast<u8*>(allocData);
			headerBytes = allocBytes + headerOffset;
			allocation = Allocation { .bytes { commit - userOffset, allocBytes + userOffset } };

			allocData = VirtualAlloc(allocData, commit, MEM_COMMIT, PAGE_READWRITE);
			assertWin32(allocData);
		}
		// Case 3 = Reserve + automatic commit
		else
		{
			void* allocData = VirtualAlloc(nullptr, reserve, MEM_RESERVE, PAGE_NOACCESS);
			assertWin32(allocData);

			allocBytes = static_cast<u8*>(allocData);
			headerBytes = allocBytes + headerOffset;
			allocation = Allocation { .bytes { reserve - userOffset, allocBytes + userOffset } };

			allocData = VirtualAlloc(allocData, commit + pageSize, MEM_COMMIT, PAGE_READWRITE);
			assertWin32(allocData);

			void* guardData = allocBytes + commit;
			u8** headerPtr = static_cast<u8**>(guardData);
			*headerPtr = headerBytes;

			guardData = VirtualAlloc(guardData, pageSize, MEM_COMMIT, PAGE_GUARD | PAGE_READWRITE);
			assertWin32(guardData);
		}
	}

	*new (headerBytes) Header {
		.id           = id,
		.headerOffset = u8(headerOffset),
		.userOffset   = u8(userOffset),
		.committed    = commit,
		.reserved     = reserve,
	};

	return allocation;
}

Allocation&
VirtualAllocator::free(Allocation& allocation)
{
	using namespace AllocatorInternal;

	Header& header = getHeader(allocation);
	void* allocData = reinterpret_cast<u8*>(&header) - header.headerOffset;
	b8 result = VirtualFree(allocData, 0, MEM_RELEASE);
	assertWin32(result);
	allocation = {};
	return allocation;
}

Allocation&
VirtualAllocator::commit(Allocation& allocation, i64 size)
{
	using namespace AllocatorInternal;

	Header& header = getHeader(allocation);
	size = roundUp<i64>(header.userOffset + size, pageSize);
	size = max(size, header.committed);
	assert(size <= header.reserved);

	// NOTE: Memory is guaranteed to be zeroed
	// NOTE: Address is rounded down to a page boundary
	// NOTE: When address is specified all touched pages are allocated
	void* allocData = reinterpret_cast<u8*>(&header) - header.headerOffset;
	allocData = VirtualAlloc(allocData, size, MEM_COMMIT, PAGE_READWRITE);
	assertWin32(allocData);

	header.committed = size;
	allocation.bytes.len = size - header.userOffset;
	return allocation;
}

#endif
