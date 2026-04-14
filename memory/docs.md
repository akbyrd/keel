# Overview

Fundamental pieces
* Allocator  - A type that describes how allocations should be performed. Has no knowledge of objects and their lifetimes.
* Allocation - An allocation made by am allocator. Raw bytes. Remembers which allocator it came from.
* Container  - A higher level type that manages object lifetimes. Uses an allocator internally.
* Object     - An individual object made by a container.

This splits things into two categories: one that deals with raw memory (allocators and allocations) and one that deals with objects (containers and objects). Programs are expected to work with containers (namely, arenas) the majority of the time. Important lifetimes are generally expected to have a container associated with them and all operations within that lifetime share the container. Containers can be nested within one another when strict sub-lifetimes are needed. Stack-based, thread-local scratch arenas are provided automatically and can be used for short lived, temporary allocations. Containers are passed to any function that needs to allocate space for its results.

Memory is value initialized (or default constructed) by default. Containers generally provide non-initializing overloads that can be used when performance is critical.

Allocators and containers have no thread synchronization. They are expected to be used on a single thread. If they must be used by multiple thread the user is responsible for proper synchronization to prevent concurrent access.



# Usage
TODO: Document



# Allocators
Allocators allocate and free raw memory from the platform. They do not deal with object lifetimes, meaning they do not call constructors or destructors. Allocations are intended to be exposed through higher level APIs such as arenas, and those higher level APIs are responsible for managing object lifetimes. This allows allocators to have minimal overhead and to be reused by different higher level APIs as needed. Allocations are expected to be large and used for many objects. Allocations store additional metadata which is small (~24 bytes), but may cause an additional page to be allocated.

## Types
TODO: Document
TODO: In-line vs out-of-line metadata variants for arenas?

### Leak Check Allocator
Metadata could be stored in a header as part of the allocation or in a separate allocation.
* Separate allocation
	* Shared between user allocations
	* Could be thread-local. If so, have to deal with alloc on one thread and free on another. Or disallow this and detect it.



# Containers

## Types
* Arena (Virtual Memory)
* Arena (Block Allocations)

### Arena
When an arena is destroyed or a scope is ended destructors will be run automatically.

If scopes are a separate type it means we always have to pass the root arena in addition to scopes (or stash it inside each scope)
	TODO: Explain
If scopes are a separate object it means we create a footgun by having two alive at the same time
	TODO: Explain
Arenas can share an allocator, but that's usually a bad idea
	TODO: Explain
TODO: Support manual alignment



# Debugging

## Fill patterns
* Allocated - 0xAA
* Freed     - 0xBB
* Invalid   - 0xCC

## Implementations

### Allocator
* Usage statistics
* Stack tracking
* Double free
* Use after free
* Overflow
* Underflow
* Thread check
* Fill patterns

### Arena
* Stack tracking
* Leak check
* Use after free
* Overflow
* Underflow
* Thread check
* Fill patterns
