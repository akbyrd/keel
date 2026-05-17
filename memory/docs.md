# Overview

Fundamental pieces
* Allocator  - A type that describes how allocations should be performed. Has no knowledge of objects and their lifetimes.
* Allocation - An allocation made by an allocator. Remembers which allocator it came from.
* Container  - A higher level type that manages object lifetimes. Uses an allocator/allocation internally.
* Object     - An individual object made by a container.

This splits things into two categories: one that deals with raw memory (allocators and allocations) and one that deals with objects (containers and objects). Programs are expected to work with containers (namely, arenas) the majority of the time. Important lifetimes are generally expected to have a container associated with them and all operations within that lifetime share the container. Containers can be nested within one another when strict sub-lifetimes are needed. Stack-based, thread-local scratch arenas are provided automatically and can be used for short lived, temporary allocations. Containers are passed to any function that needs to allocate space for its results.

Allocators are always thread-safe and can be used from multiple threads without manual synchronization. Allocators are generally expected to avoid the need for internal synchronization, but exceptions may exist, particularly for debug allocators that track additional metadata.



# Allocators
Allocators allocate and free raw memory from the platform. They do not deal with object lifetimes, meaning they do not call constructors or destructors. Allocations are intended to be exposed through higher level APIs such as arenas, and those higher level APIs are responsible for managing object lifetimes. This allows allocators to have minimal overhead and to be reused more easily. Allocations are expected to be large and used for many objects. Allocations store additional metadata which is small (~24 bytes), but may cause an additional page to be allocated.

## Types
There is currently only one allocator type: VirtualAllocator. This uses platform APIs to allocate memory. Platforms are generally expected to support virtual memory, but are not required to. Functions that require virtual memory will not compile on platforms that don't support them.

## Debugging
No debugging facilities currently exist for allocators.
