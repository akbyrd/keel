# Multiple Allocator Types
It's unclear whether there's a need for multiple allocator types. Most platforms support virtual memory and that's a reasonable default allocator. What other allocator types might be useful?

## Stack Allocator
A stack allocator seems ill-advised because of limited stack size. Scoped arenas are a better tool for this pattern.

## GPU Allocator
A GPU allocator is niche enough that it probably doesn't need to be part of a general purpose allocator abstraction.



# Debug Allocators
An appealing idea is debug allocators that check for various issues, such as memory leaks, overflow, etc.

## Allocators or Containers
The first question is whether the debug checks are part of the allocator or the container. Since we split these concepts and expect almost all allocations to be from a container, that's where we want the debug checks to be performed.

## Leak Check
We expect that approximately all allocations go through arenas, which manage the lifetime of all allocations within them as a unit. Thus to check for leaks we need to find the set of active arenas. We can do this by maintaining a list of active arenas. But we expect this type of checking to be less useful because there are far fewer arenas than individual objects. There will be fewer opportunities to create leaks and they will be easier to notice.

## Overflow and Underflow
Overflow and underflow checks can be useful on arenas, but not on arrays. Arrays would need to insert padding between elements, which breaks the expectation that elements are laid out contiguously. This requires the usage code to know about a wrapper struct around elements, or use an indirection that hides this. Neither are desirable.



# Wrapping Containers
Another appealing idea is being able to wrap a container and add debug functionality to it without rewriting it and without changing user code. However, there are various concerns and design issues that arise.

## Allocator Dependence
Some debug behavior actually depends on the specific type of allocator. An overflow detecting allocator may want to move each allocation to the end of a page, reserve the next page, and disable access to it. This only works with a virtual allocator and a different allocator type would need a different approach.

## Performance
Wrapping requires some form of indirection. There is a performance cost to dynamic dispatch and allocation is expected to be a hot path.
