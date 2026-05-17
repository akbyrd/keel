# Overview
This library aims to be a standard library replacement that focuses on simplicity, readability, and compile time. It is not intended to be a drop-in replacement and deviates massively from the design of standard library. Notably, object allocations are generally handled with arenas, all containers store data contiguously, and spans are the primary abstraction for passing objects to functions.

Explicitness is always preferred over implicitness. Allocations are never hidden behind innocent operations such as assignment.

Robustness in the face of all possible edge cases is not a goal. When in tension with our other goals, robustness is sacrificed. The library will attempt to cover all cases up until the complexity-to-functionality ratio becomes (subjectively) unfavorable. That said, limitations should not be a result of carelessness. All edge cases are expected to be carefully considered and a value judgement applied based on the cost-benefit ratio. All limitations will be explicitly documented.

While this codebase is decided not "modern C++", we do use newer features where they enable our goals instead of older, more complicated solutions. Examples include concepts (instead of SFINAE), constexpr (instead of template metaprogramming), deducing this (instead of overload sets), and modules (to improve compile time).



# Supported Configurations
## Platforms
Windows

## ISAs
x86, x86-64

## Compilers
msvc, clang, clang-cl



# Parts
* Core       - compiler and platform abstraction, logging, etc
* Containers - arenas, data structures, etc
* Math       - math functions, RNG, etc
* Memory     - allocators, byte operations, lifetime operations
