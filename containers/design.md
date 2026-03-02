basics
* all containers are contiguous data + optional metadata
* always implemented as len + allocator + metadata
* always convertible to span
* fixed size metadata is bundle into the allocation
* metadata includes allocator itself
* four iterators: { fast, safe } x { forward, reverse }
* safe iterators track invalidation and check bounds

included
* array        = len + allocator - metadata
* set          = len + allocator - metadata
* map          = len + allocator + metadata
* arena        = len + allocator + metadata
* string       = len + allocator - metadata
* cstring      = len + allocator - metadata
* stable array = TODO: Maybe?
* sparse array = TODO: Maybe?
* block array  = len + allocator + metadata
* single list  = len + allocator - metadata
* double list  = len + allocator - metadata
* heap         = alias for new/delete (TLSF, ...)
* ring         =

support types
* container = len + allocator
* span      = ptr + len
* allocator = ptr + cap

container is responsible for metadata, not the allocator
* Containers have more context. e.g. arrays don't need metadata because they know the type statically

what about fully decoupling containers and allocators?
* containers are a way of interpreting an allocation
* allocations know about their allocator
