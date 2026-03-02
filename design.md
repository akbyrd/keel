TODO: Think about the design-style split



# Language
C++ 20

Specification
PDF:  https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/n4861.pdf
HTML: https://timsong-cpp.github.io/cppwp/n4861/



# Platforms
Assume 64-bit, no 32-bit support.
TODO: WebAssembly is 32-bit



# Indices
Indices are signed. This is to avoid making reverse for loops less convenient.
```
for (i32 i = i32(array.len); i >= 0; --i)
	array[u32(i)];
```
Indices are 64 bits. For types that contain a length and a pointer like spans the padding next to a 32-bit index would result in the same total footprint. Types that contain a length, capacity, and pointer will end up 64 bits larger. 32-bit platforms will take a larger hit, but I estimate <5% of relevant CPUs in use in 2025 are 32-bit.



# Const
* Use logical const, not physical const.
* `const_cast` is not allowed.



# Parts
* Platform Abstraction
* Compiler Abstraction
* Type Metadata
* Memory Management
	* Allocators
	* Byte Operations
	* Lifetime Operations
* Containers
* Math
* RNG
* Logging
* Testing
