# Overview
* All containers are contiguous data + optional metadata
* Always convertible to span
* Four iterators: { fast, safe } x { forward, reverse }
* Safe iterators track invalidation and check bounds



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
