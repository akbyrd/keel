# Arenas

## TODO
* Consider adding a way to release the allocation from the arena
* Profile branch vs SEH for auto-commit behavior
* Compare in-line vs out-of-line metadata
* Handle scoping
	* Separate type - have to pass the root arena in addition to scopes (or stash it inside each scope)
	* Separate object - footgun by having two alive at the same time
* Debugging
	* Stack tracking
	* Leak check
	* Use after free
	* Overflow
	* Underflow
	* Thread check
	* Fill patterns
* Can we nest arenas?
	* How do we ensure children end their lifetime before parents?
	* Do arenas need to track their parent? (Debug only?)

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

## Allocation Ownership

3 axes to consider:
* Who allocates? The user or the arena?
* How is the allocation transferred? Copied or moved?
* Who frees? The user or the arena?

Copying an allocation seems like a bad idea. It's a footgun to have two allocation structs pointing the same actual allocation.
The arena needs ownership of the allocation because it commits as needed, which mutates the allocation.
Moving the allocation avoid duplicating the allocator API on the arena, but it requires move semantics for allocations.

### Transferring Ownership
* CRTP
	* Con: Not possible
* In class
	* Con: Boilerplate if using manual assignment
	* Con: Bad error message if using private copy
	* Con: Visual noise for users
	* Con: No longer an aggregate
	* Con: What happens to current allocation?
* Wrapper
	* NOTE: Best so far
	* Con: Requires value is movable
	* Con: Requires value is default constructible
	* Con: move(alloc) doesn't clear the allocation
* In arena
	* Con: move(alloc) doesn't clear the allocation
* Arena doesn't take ownership
	* NOTE: Also pretty good
	* Con: User can accidentally mutate allocation
* Arena does the allocation
	* Con: Duplicate allocator API
* Kitchen sink
	* NOTE: Best so far?
	* Pass in bytes, move in allocation, or do allocation internally

* dtor - assert, but don't free?
	* Maybe this happens in VirtualAllocator
	* Arenas should be able to work on any chunk of memory, may not be an allocation

## Allocator Interactions
Arenas have a lot of dependencies on the allocator
* Block allocator requires a list of blocks
* Realloc requires a branch to skip relocation
* Virtual allocator requires a branch to skip relocation
* Overflow/underflow allocator requires custom offset on objects

## Usage Patterns

### Usage Pattern 1
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

### Usage Pattern 2
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

### Usage Pattern 3
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



# Arrays

## TODO
* Maybe arrays are a special case of arenas?
