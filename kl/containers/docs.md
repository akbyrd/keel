# Overview

Where allocators are the low level tool that manage raw bytes, containers are the high level tool that manage objects. Containers use memory (that may or may not be provided by an allocator) and decide how to organize objects and metadata within it.

* All containers are contiguous data + optional metadata
* Always convertible to span
* Four iterators: { fast, safe } x { forward, reverse }
* Safe iterators track invalidation and check bounds
* Objects are value-initialized by default. Containers generally provide non-initializing overloads that can be used when performance is critical.
* Containers are not thread-safe. If they must be used by multiple threads the user is responsible for proper synchronization to prevent concurrent access.



# Types
* Arena
* Array
* CString
* Heap
* List (Double)
* List (Single)
* Map
* Ring
* Set
* String

## Arena
When an arena is destroyed or a scope is ended destructors will be run automatically.



# Views
* Span
* StringSpan
