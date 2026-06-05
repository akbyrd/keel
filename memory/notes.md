# Allocators

## TODO
* Do we want unaligned alloc?
* Investigate cache thrashing.
	4 kB allocations will alias the same cache set, potentially thrashing.
	Compare performance of different allocators in a real workload.
	Might not be an issue since a small number of arenas will soak up most allocations and won't alias.
	But if many small allocations are created they are going to end up in the same cache set.
	5850x has a 32 kB 8-way cache. Would need >8 allocations in active use to begin thrashing.
	But even without them being used at the same time L1 hit rate is going to be lower.
	Aliasing happens at 32 kB / 8 = 4 kB increments. This is nearly universal.
	Could use a random offset on allocations, but memory overhead would be high.
* Decide on error handling
* Decide on testing strategy (toggle between asserts and error codes?)
* Create a user-overridable failure function
* How do we initialize allocators (e.g. unchanging data like page size)?
* Why different types for size and align?
* Want a way to calculate overhead
* Large pages?
* Are allocations RAII?
* Default allocator (alias that can be swapped out)
* Hook new
* Debugging
	* Usage statistics
	* Stack tracking
	* Double free
	* Use after free
	* Overflow
	* Underflow
	* Thread check
	* Fill patterns

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

## Design Axes
* Where the memory comes from - virtual, stack, gpu
* How the memory is allocated - no reserve, reserve and manual commit, reserve and automatic commit
* How objects are allocated   - stack, fixed size bins, free list, occupancy list
The first two are the purview of memory allocators. The last is determined by the container.

## Usage / Configuration
* Who performs the allocation      - callee or caller
* Which debug features are enabled - leak check, use after free, thread check, etc
* Can debug checks be combined     - leak + thread, etc

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


# Allocations
## TODO
* Allocate exactly what was asked for and let users add the metadata size if desired
* ScopedAllocation



# Realloc vs extend
TODO: Update this section. No longer true? Would it be on allocators or containers?
A realloc operation does not exist for allocators. Instead, an extend operation is provided. Reallocating requires knowledge of objects in order to relocate, which allocators do not have. This means realloc is a container operation, not an allocator operation. Extend is a failable operation that attempts to grow the allocation in-place. When it fails the container can perform an alloc-relocate-free sequence.



# Composition
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



# Multiple Allocator Types
It's unclear whether there's a need for multiple allocator types. Most platforms support virtual memory and that's a reasonable default allocator. What other allocator types might be useful?

## Stack Allocator
A stack allocator seems ill-advised because of limited stack size. Scoped arenas are a better tool for this pattern.

## GPU Allocator
A GPU allocator is niche enough that it probably doesn't need to be part of a general purpose allocator abstraction.



# Debug Allocators
An appealing idea is debug allocators that check for various issues, such as memory leaks, overflow, etc.

## Types
* Fill patterns
* Leak check
* Thread check
* Overflow
* Underflow
* Use after free
* Lifetime check
* Sanitizer hooks

## TODO
* How do we combine headers / support different metadata sizes?
	1. Two headers
		Pro: Common case remains ideal (non wrapped allocator)
		Con: Dependent load
	2. Fixed / max size header
		Con: Limits extensibility
		Con: Wasted space when unused
	3. CRTP
		Base allocators know their most derived type
		Con: All allocators are templates
	4. Allocator extensibility
		Type traits to inject additional behavior
		Con: Probably hard to understand / fragile
	5. Templated allocator
		Template options to enable/disable features
		Pro: Possibly the simplest option. Feels like a good fit.
		Con: Requires implementation to be in a header.
			Or, explicit instantiation for all variants?
	6. Separate allocator types (no wrapping)
		Con: Some code duplication (fine)
		Con: Still have to deal with different header sizes (dual header approach?)

## Allocators or Containers
The first question is whether the debug checks are part of the allocator or the container. Since we split these concepts and expect almost all allocations to be from a container, that's where we want the debug checks to be performed.

## Leak Check
We expect that approximately all allocations go through arenas, which manage the lifetime of all allocations within them as a unit. Thus to check for leaks we need to find the set of active arenas. We can do this by maintaining a list of active arenas. But we expect this type of checking to be less useful because there are far fewer arenas than individual objects. There will be fewer opportunities to create leaks and they will be easier to notice.

Implemented as a linked list of active allocations.
Where do we store it?
1. Thread-local storage
	* How do we handle alloc on one thread and free on another?
		* Check current thread id, synchronize only when on a different thread, steal free list entries from other threads
	* Pro: Less waste due to alignment if alignof(Header) > alignof(Node)
	* Pro: Can have variable stack size
	* Pro: Potential for stack deduplication (at a performance cost)
	* Pro: Potential for shared logic (other tracking, such at categorization)
	* Con: Indirect lookup, not in same cache line
2. Additional header
	* Where do we put the head node? TLS?
	* How do we handle multiple threads touching allocator?
	* Con: Indirect lookup, but mostly in same cache line

## Overflow and Underflow
Overflow and underflow checks can be useful on arenas, but not on arrays. Arrays would need to insert padding between elements, which breaks the expectation that elements are laid out contiguously. This requires the usage code to know about a wrapper struct around elements, or use an indirection that hides this. Neither are desirable.

### Underflow
Can this be implemented as a wrapper?
Cannot compose with an overflow allocator (fundamental, can't be aligned to both the beginning and end of a page)
Cannot compose with arenas (incidental, there's only one real allocation)
Behavior:
* Allocate an extra page before each allocation
* Disallow access to the page
* Install a fault handler
* Align each allocation to a page boundary


# Wrapping Containers
Another appealing idea is being able to wrap a container and add debug functionality to it without rewriting it and without changing user code. However, there are various concerns and design issues that arise.

## Allocator Dependence
Some debug behavior actually depends on the specific type of allocator. An overflow detecting allocator may want to move each allocation to the end of a page, reserve the next page, and disable access to it. This only works with a virtual allocator and a different allocator type would need a different approach.

## Performance
Wrapping requires some form of indirection. There is a performance cost to dynamic dispatch and allocation is expected to be a hot path.



# C++ Minutiae
Object lifetimes can be started by new (including placement new), unsigned char/byte arrays, assigning to an implicit-lifetime object, and blessed library functions (memcpy, malloc, create_at, start_lifetime_as, etc), and potentially blessed implementation functions (mmap, VirtualAlloc, etc).

## Concepts and Keywords
* Pointer provenance
* Implicit-lifetime objects
* Pointer-interconvertible objects
* Common initial sequence
