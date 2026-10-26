#pragma once

#include "core/platform.hpp"

#if KL_PLATFORM_WIN32

#include "core/math.hpp"
#include "core/win32.hpp"
#include "memory/allocator.hpp"

namespace kl
{
	namespace AllocatorInternal
	{
		static inline constexpr u16 typeId           = 0xA11C;
		static inline constexpr u8  maxAlign         = 64;
		static inline constinit u32 pageSize         = 0;
		static inline constinit u32 allocGranularity = 0;

		struct Header
		{
			u16 typeId; // TODO: Implement a proper type id
			u8  headerOffset;
			u8  userOffset;
			i64 committed;
			i64 reserved;
		};

		static const Header& getHeader(const Allocation& alloc)
		{
			assert(alloc.bytes.data);
			i64 userAddr   = i64(alloc.bytes.data);
			i64 headerAddr = roundDown<i64>(userAddr - sizeof(Header), alignof(Header));

			Header& header = *reinterpret_cast<Header*>(headerAddr);
			assert(header.typeId == typeId);
			return header;
		}

		static Header& getHeader(Allocation& alloc)
		{
			const Allocation& constAlloc = alloc;
			const Header& header = getHeader(constAlloc);
			return const_cast<Header&>(header);
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

		return true;
	}

	Allocation
	VirtualAllocator::allocate(i64 size, u8 align)
	{
		return reserve(size, size, align);
	}

	Allocation
	VirtualAllocator::reserve(i64 reserve, i64 commit, u8 align)
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
		void* allocData = nullptr;
		if (reserve > commit)
		{
			allocData = VirtualAlloc(nullptr, reserve, MEM_RESERVE, PAGE_NOACCESS);
			assertWin32(allocData);

			allocData = VirtualAlloc(allocData, commit, MEM_COMMIT, PAGE_READWRITE);
			assertWin32(allocData);
		}
		else
		{
			allocData = VirtualAlloc(nullptr, commit, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			assertWin32(allocData);
		}

		u8* allocBytes = static_cast<u8*>(allocData);
		auto& header = *new (allocBytes + headerOffset, PlacementTag{}) Header {};
		header = Header {
			.typeId       = typeId,
			.headerOffset = u8(headerOffset),
			.userOffset   = u8(userOffset),
			.committed    = commit,
			.reserved     = reserve,
		};

		return Allocation { Span<u8> { commit - userOffset, allocBytes + userOffset } };
	}

	void
	VirtualAllocator::commit(Allocation& alloc, i64 size)
	{
		using namespace AllocatorInternal;

		Header& header = getHeader(alloc);
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
		alloc.bytes.len = size - header.userOffset;
	}

	void
	VirtualAllocator::free(Allocation& alloc)
	{
		using namespace AllocatorInternal;

		Header& header = getHeader(alloc);
		void* allocData = reinterpret_cast<u8*>(&header) - header.headerOffset;
		b8 result = VirtualFree(allocData, 0, MEM_RELEASE);
		assertWin32(result);
		alloc = {};
	}

	i64
	VirtualAllocator::getReserved(const Allocation& alloc)
	{
		using namespace AllocatorInternal;

		const Header& header = getHeader(alloc);
		return header.reserved;
	}
}

#endif
