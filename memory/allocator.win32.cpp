#pragma once

#include "core/platform.hpp"

#if BLD_PLATFORM_WIN32

#include "core/math.hpp"
#include "core/win32.hpp"
#include "memory/allocator.hpp"

// -------------------------------------------------------------------------------------------------
// Shared

namespace AllocatorInternal
{
	static inline constexpr u8  maxAlign         = 64;
	static inline constinit u32 pageSize         = 0;
	static inline constinit u32 allocGranularity = 0;

	struct Header
	{
		// TODO: Free function ptr? Or switch on type?
		// TODO: struct TypeId? struct AllocTypeIndex?
		// TODO: Rename
		u32 magic;
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
		return header;
	}
}

// -------------------------------------------------------------------------------------------------
// VirtualAllocator

namespace VirtualAllocatorInternal
{
	using namespace AllocatorInternal;

	static inline constexpr u32 magic = 0xA11C'1111;
}

constinit Allocator
VirtualAllocator::interface = Allocator {
	.alloc = alloc,
	.free  = free,
};

b8
VirtualAllocator::init()
{
	using namespace VirtualAllocatorInternal;

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
VirtualAllocator::alloc(i64 size, u8 align)
{
	return alloc(size, size, align);
}

Allocation
VirtualAllocator::alloc(i64 reserve, i64 commit, u8 align)
{
	// Layout: [Slack | Header | Data]

	using namespace VirtualAllocatorInternal;

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
	auto& header = *new (allocBytes + headerOffset) Header {};
	header = Header {
		.magic        = magic,
		.headerOffset = u8(headerOffset),
		.userOffset   = u8(userOffset),
		.committed    = commit,
		.reserved     = reserve,
	};

	return Allocation {
		.bytes = { commit - userOffset, allocBytes + userOffset },
	};
}

Allocation&
VirtualAllocator::free(Allocation& allocation)
{
	using namespace VirtualAllocatorInternal;

	Header& header = getHeader(allocation);
	assert(header.magic == magic);

	void* allocData = reinterpret_cast<u8*>(&header) - header.headerOffset;
	b8 result = VirtualFree(allocData, 0, MEM_RELEASE);
	assertWin32(result);
	allocation = {};
	return allocation;
}

Allocation&
VirtualAllocator::commit(Allocation& allocation, i64 size)
{
	using namespace VirtualAllocatorInternal;

	Header& header = getHeader(allocation);
	assert(header.magic == magic);

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

// -------------------------------------------------------------------------------------------------
// LeakCheckAllocator

// TODO: Refine this
namespace LeakCheckAllocatorInternal
{
	using namespace AllocatorInternal;

	static inline constexpr u32 magic = 0xA11C'2222;

	// TODO: Rename
	struct Node
	{
		// TODO: Decide on frame count
		static constexpr u32 maxFrames = 8;

		Node* prev;
		Node* next;
		void* frames[maxFrames];
	};

	// TODO: -Wno-missing-designated-field-initializers
	thread_local Node head = {
		.prev = &head,
		.next = &head,
	};

	static Node& getNode(Header& header)
	{
		i64 nodeAddr = roundDown(i64(&header) - i64(sizeof(Node)), i64(alignof(Node)));
		return *reinterpret_cast<Node*>(nodeAddr);
	}

	static void insert(Node& node)
	{
		node = Node {
			.prev = head.prev,
			.next = &head,
		};
		// TODO: Decide on from skipping
		// TODO: Does this zero unused frames?
		RtlCaptureStackBackTrace(1, Node::maxFrames, node.frames, nullptr);

		head.prev = &node;
		head.next->prev = &node;
	}

	static void remove(Node& node)
	{
		node.prev->next = node.next;
		node.next->prev = node.prev;
	}
};

constinit Allocator LeakCheckAllocator::interface = Allocator {
	.alloc = alloc,
	.free  = free,
};

b8 LeakCheckAllocator::init()
{
	using namespace LeakCheckAllocatorInternal;

	if (!isMultipleOf<u32>(allocGranularity, alignof(Node)))
	{
		// TODO: logging
		return false;
	}

	return true;
}

// TODO: Try to make this a wrapper and not copy the implementation
Allocation LeakCheckAllocator::alloc(i64 size, u8 align)
{
	// Layout: [Slack | Node | Header | Data]

	using namespace LeakCheckAllocatorInternal;

	assert(isPowerOf2(align));
	assert(align <= maxAlign);
	assert(isMultipleOf<u32>(allocGranularity, align));

	// Find the minimum offset for user data, respecting all alignment
	i64 nodeOffset   = 0; // we know allocGranularity is a multiple of alignof(Node)
	i64 headerOffset = roundUp<i64>(nodeOffset   + sizeof(Node),   alignof(Header));
	i64 userOffset   = roundUp<i64>(headerOffset + sizeof(Header), align);

	// Shift node and header to the right (when user align is larger)
	headerOffset = roundDown<i64>(userOffset   - sizeof(Header), alignof(Header));
	nodeOffset   = roundDown<i64>(headerOffset - sizeof(Node),   alignof(Node));

	assert(userOffset   <= maxVal<i64> - size);
	assert(userOffset   <  maxVal<u8>);
	assert(headerOffset <  maxVal<u8>);

	i64 bytes = roundUp<i64>(userOffset + size, pageSize);

	// NOTE: Memory is guaranteed to be zeroed
	void* allocData = VirtualAlloc(nullptr, bytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	assertWin32(allocData);

	u8* allocBytes = static_cast<u8*>(allocData);
	auto& header = *new (allocBytes + headerOffset) Header {};
	header = Header {
		.magic        = magic,
		.headerOffset = u8(headerOffset),
		.userOffset   = u8(userOffset),
		.committed    = bytes,
		.reserved     = bytes,
	};

	Node& node = getNode(header);
	insert(node);

	return Allocation {
		.bytes = { bytes - userOffset, allocBytes + userOffset },
	};
}

Allocation& LeakCheckAllocator::free(Allocation& allocation)
{
	using namespace LeakCheckAllocatorInternal;

	Header& header = getHeader(allocation);
	assert(header.magic == magic);

	Node& node = getNode(header);
	remove(node);

	void* allocData = reinterpret_cast<u8*>(&header) - header.headerOffset;
	b8 result = VirtualFree(allocData, 0, MEM_RELEASE);
	assertWin32(result);
	allocation = {};
	return allocation;
}

#endif
