TODO: Split into "docs" and "notes"

# Overview

Fundamental pieces
* Allocator  - A type that describes how allocations should be performed. Has no knowledge of objects and their lifetimes.
* Allocation - An allocation made by am allocator. Raw bytes. Remembers which allocator it came from.
* Container  - A higher level type that manages object lifetimes. Uses an allocator internally.
* Object     - An individual object made by a container.

This splits things into two categories: one that deals with raw memory (allocators and allocations) and one that deals with objects (containers and objects). Programs are expected to work with containers (namely, arenas) the majority of the time. Important lifetimes are generally expected to have a container associated with them and all operations within that lifetime share the the container. Containers can be nested within one another when strict sub-lifetimes are needed. Stack-based, thread-local scratch arenas are provided automatically and can be used for short lived, temporary allocations. Containers are passed to any function that needs to allocate space for its results.

TODO: Is memory zeroed?
TODO: Thread safety



# Allocators
Allocators allocate and free raw memory from the platform. They do not deal with object lifetimes, meaning they do not call constructors or destructors. Allocations are intended to be exposed through higher level APIs such as arenas, and those higher level APIs are responsible for managing object lifetimes. This allows allocators to have minimal overhead and to be reused by different higher level APIs as needed. Allocations are expected to be large and used for many objects. Allocations store additional metadata which is small (~24 bytes), but may cause an additional page to be allocated.


## Requirements
* Must be swappable. Allocators are an abstraction and allow users to select which specific allocator is used without affecting code that uses the allocator.
* Must support user allocators. Users should be able to implement custom allocators that conform to the abstraction and replace the default allocator.
* Must be able to wrap allocators. A basic allocator should be able to be wrapped with debug functionality without needing to duplicate the allocation behavior. For example, a platform virtual allocator should be able to be wrapped with a leak check allocator in a way that composes these two behaviors without duplicating them. There may be special cases where this is not possible.
* An allocation is a specific type, not a raw pointer. This avoids ambiguity (i.e. "is this pointer an allocation?").
* Allocators are not part of the type of the types that use them (i.e. containers are not templated on the type of allocator they use).
* Allocators are intrinsically thread-safe and do not perform synchronization between threads. Any state must be stored within allocations themselves or in thread-local storage.
* Allocators are always chosen by the caller. They must be passed into functions or set via thread-local context. Code should never hard code usage of a specific allocator.
* Allocations can be freed without knowing which allocator was originally used. Allocations must store a reference to the allocator that created them.
* Separate allocators can be used for returned data and temporary data.
* Allocations may return more memory than requested (but not less).


## Composition
TODO: Describe
```cpp
// Con: Doesn't say which allocator to wrap
Allocation alloc = DebugAllocator::alloc(1 * Megabyte, 8);
```
```cpp
// Con: Doesn't support unique allocators
Allocator inner = VirtualAllocator::interface;
Allocation alloc = DebugAllocator::alloc(inner, 1 * Megabyte, 8);
```
```cpp
// Pro: Can specialize for unique allocators / interactions
// Con: Simpler to just take the allocator as a parameter - that's what it resolves to internally anyway
// Con: Forces inner allocators header to be defined in a header (functions are defined in header and need to use the header struct)
Allocation alloc = DebugAllocator<VirtualAllocator>::alloc(1 * Megabyte, 8);
```
```cpp
// Con: Trickier to allow specialization
// Con: Not sure there's any advantage
using DebugAllocator = ComposedAllocator<VirtualAllocator, FillAllocator>;
```


## Types
TODO: Document
TODO: In-line vs out-of-line metadata variants for arenas?

### Leak Check Allocator
Metadata could be stored in a header as part of the allocation or in a separate allocation.
* Separate allocation
	* Shared between user allocations
	* Could be thread-local. If so, have to deal with alloc on one thread and free on another. Or disallow this and detect it.


## Metadata

### Requirements
* No metadata is stored in the Allocation struct.
* Different allocators support different metadata.
* Concrete, non-wrapped allocators load metadata with a single non-dependent load.
* Metadata can be extended when wrapping an allocator.

### Possible Implementations

#### Store metadata at the front of the allocation
Return an opaque struct.
* Pro: Caller only stores a pointer-sized object.
* Pro: Can always figure out where an allocation came from with just a pointer.
* Pro: Changing the allocator doesn't require code that uses the allocation to change (same type).
* Pro: More flexibility to change implementation details.
* Con: Indirection to get allocation metadata.
* Con: Debuggers sometimes have issues showing details of opaque types.

#### Return a struct with metadata
* Pro: No indirection to access metadata.
* Con: Changing the allocator requires code that uses the allocation to change (different type)
* Con: It's easier for users to modify or stomp the metadata and corrupt it.

#### Return a struct with common metadata, store unique metadata at the front of the allocation
* Pro: Can still figure out where an allocation came from with just a pointer (but might store redundant data).
* Pro: Retains flexibility to change implementation details.
* Pro: No indirection to access metadata.
* Pro: Changing the allocator doesn't require code that uses the allocation to change (same type).
* Con: Combines the complexity of both approaches.
* Con: Caller stores a larger object.
* Con: Class of bugs - user passes wrong metadata (or modifies it).


## Other Notes

### Design axes
* Where the memory comes from - virtual, stack
* How the memory is allocated - no reserve, reserve and manual commit, reserve and automatic commit
* How objects are allocated   - stack, fixed size bins, free list, occupancy list

The first two are the purview of memory allocators. The last is determined by the object allocator.

### Realloc vs extend
TODO: Update this section. No longer true? Would it be on allocators or containers?
A realloc operation does not exist for allocators. Instead, an extend operation is provided. Reallocating requires knowledge of objects in order to relocate, which allocators do not have. This means realloc is a container operation, not an allocator operation. Extend is a failable operation that attempts to grow the allocation in-place. When it fails the container can perform an alloc-relocate-free sequence.


## Debugging

### Fill patterns
* Allocated - 0xAA
* Freed     - 0xBB
* Invalid   - 0xCC

### Implementations
* Stack tracing
* Leak check
* Double free
* Use after free
* Overflow
* Underflow
* Thread check
* Fill bytes


## C++ Minutiae
Object lifetimes can be started by new (including placement new), unsigned char/byte arrays, assigning to an implicit-lifetime object, and blessed library functions (memcpy, malloc, create_at, start_lifetime_as, etc), and potentially blessed implementation functions (mmap, VirtualAlloc, etc).

### Concepts and Keywords
* Pointer provenance
* Implicit-lifetime objects
* Pointer-interconvertible objects
* Common initial sequence



# Arenas
When an arena is destroyed or a scope is ended destructors will be run automatically.

If scopes are a separate type it means we always have to pass the root arena in addition to scopes (or stash it inside each scope)
	TODO: Explain
If scopes are a separate object it means we create a footgun by having two alive at the same time
	TODO: Explain
Arenas can share an allocator, but that's usually a bad idea
	TODO: Explain
TODO: Support manual alignment


## Lifetimes
Permanent
Frame
Stack


## Metadata
Metadata is only needed for objects with non-trivial destructors. Metadata is placed before objects when they are pushed into the arena. Only one metadata is needed for an array of objects of the same type. Pushing objects of the same type one-at-a-time will update the existing metadata rather than creating a new one.

### Alternatives
There are multiple ways we could store metadata.

#### In front of each object / array of objects
* This is the current approach
* Pro: Minimal overhead when unused. Just a single pointer memory for the internally linked list used with the metadata.
* Con: Higher overhead due to linked list.
* Con: Harder to calculate metadata overhead.
* Con: More overhead from padding between metadata and objects.
* Con: Metadata pollutes the cache when accessing objects.
* Con: Impacts alignment

#### All metadata before objects
* Pro: Easy to calculate metadata overhead.
* Pro: Minimal overhead when unused. Likely need to store the current size of the metadata so we know where to put the next one. (The offset to the first object would most likely be stored in the metadata. The capacity available for metadata could be inferred from the total size of the allocation).
* Pro: Metadata doesn't pollute the cache when accessing objects.
* Con: Metadata for an allocation is far away from the data itself. Likely a cache miss.
* Con: Requires an extra pointer or a search to find the metadata. If we use a pointer we've just put metadata in front of each object anyway.
* Con: Requires objects to be relocated if we run out of space for the metadata (and allocators don't know about objects).
* Con: Prevents pointer stability (because of the relocation above).
* Con: Impacts alignment

#### In a separate allocation
* Pro: Easy to calculate metadata overhead.
* Pro: Metadata doesn't pollute the cache when accessing objects.
* Pro: Doesn't impact alignment
* Con: Metadata for an allocation is far away from the data itself. Likely a cache miss.
* Con: Exacerbates cache aliasing because each allocation is page aligned.


## Usage

### Alternatives

#### Usage Pattern 1
* Pass by value, free when scope ends
* Creating scopes should be explicit because copying arenas should not be allowed
* Scopes must be the same type, otherwise you can't create sub-scopes
* Sub-scopes get created internally
	* There's a difference between passing a temp allocator and a return allocator
	* Makes sense to create temp allocator as sub scope of return allocator inside the callee
	* Maybe StackArena is a special type that wraps Arena and hides push
* Con: Two scopes alive at the same time is a footgun
```cpp
void foo()
{
	Arena a {};

	a.push<int>(1);
	// { 1 }

	// Arena b = a.scope(); BUG!
	{
		Arena b = a.scope();
		bar(b);
		// { 1 }
	}

	baz(a);
	// { 1, 4 }

	a.push<int>(5);
	// { 1, 4, 5 }
}

void bar(Arena b)
{
	b.push<int>(2);
	// { 1, 2 }

	{
		// b.push<int>(-1); BUG!

		Arena c = b.scope();
		c.push<int>(3);
		// { 1, 2, 3 }
	}
}

void baz(Arena& a)
{
	a.push<int>(4);
	// { 1, 4 }
}
```

#### Usage Pattern 2
* Scopes are different type
* Pass arena
* Create scopes explicitly
* Con: Have to pass scope explicitly to return data
```cpp
void foo()
{
	Arena arena {};
	ArenaScope a = arena.scope();

	a.push<int>(1);
	// { 1 }

	bar(arena);
	// { 1 }

	baz(arena, a);
	// { 1, 4 }

	a.push<int>(5);
	// { 1, 4, 5 }
}

void bar(Arena& arena)
{
	ArenaScope b = arena.scope();
	b.push<int>(2);
	// { 1, 2 }

	{
		ArenaScope c = arena.scope();
		c.push<int>(3);
		// { 1, 2, 3 }
	}
}

void baz(Arena& arena, ArenaScope& a)
{
	a.push<int>(4);
	// { 1, 4 }
}
```

#### Usage Pattern 3
* Push happens on same object
* Seems like the best option
```cpp
void foo()
{
	Arena a {};

	a.push<int>(1);
	// { 1 }

	bar(a.scope());
	// { 1 }

	baz(a);
	// { 1, 4 }

	a.push<int>(5);
	// { 1, 4, 5 }
}

void bar(Arena& b)
{
	b.push<int>(2);
	// { 1, 2 }

	{
		ArenaScope s = b.scope();
		b.push<int>(3);
		// { 1, 2, 3 }
	}
}

void baz(Arena& a)
{
	a.push<int>(4);
	// { 1, 4 }
}
```

With allocator
```cpp
void foo()
{
	Allocator alloc {};
	Arena a { alloc };
	Arena b { alloc }; // allowed, usually a bad idea

	a.push<int>(1);
	// { 1 }

	bar(a.scope());
	// { 1 }

	baz(a);
	// { 1, 4 }

	a.push<int>(5);
	// { 1, 4, 5 }
}

void bar(Arena& b)
{
	b.push<int>(2);
	// { 1, 2 }

	{
		ArenaScope s = b.scope();
		b.push<int>(3);
		// { 1, 2, 3 }
	}
}

void baz(Arena& a)
{
	a.push<int>(4);
	// { 1, 4 }
}
```

With context
```cpp
struct Context
{
	Arena perm;
	Arena stack;
};

void foo()
{
	Context ctx {};
	ctx.perm  = Allocator {};
	ctx.stack = Allocator {};

	ctx.stack.push<int>(1);
	// { 1 }

	ctx.stack.pushScope();
	bar(ctx);
	ctx.stack.popScope();
	// { 1 }

	{
		ctx.stack.pushScope();
		defer { ctx.stack.popScope(); };
		bar(ctx);
		// { 1 }
	}

	{
		ArenaScope s = ctx.stack.scope();
		bar(ctx);
		// { 1 }
	}

	with (ArenaScope s = ctx.stack.scope())
	with_auto (ctx.stack.scope())
	{
		bar(ctx);
		// { 1 }
	}

	baz(ctx);
	// { 1, 4 }

	ctx.stack.push<int>(5);
	// { 1, 4, 5 }
}

void bar(Context& ctx)
{
	ctx.stack.push<int>(2);
	// { 1, 2 }

	{
		ArenaScope s = ctx.stack.scope();
		ctx.stack.push<int>(3);
		// { 1, 2, 3 }
	}
}

void baz(Context& ctx)
{
	ctx.stack.push<int>(4);
	// { 1, 4 }
}
```


## References
https://www.rfleury.com/p/enter-the-arena-talk
Arenas are each a stack.
Functions accept arena as parameter.
