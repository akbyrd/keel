#pragma once

#include "containers/span.hpp"
#include "core/math.hpp"
#include "memory/lifetime.hpp"

struct TypeId
{
	u32 val;
};

template <typename T>
constexpr TypeId id = []()
{
	// TODO: Want this to be random for better hashing, but guaranteed not to conflict.
	// Need build system support.
	// Generate hashes and confirm unique
	// What stability properties are required?
	// Seed with local path? Project name? Custom?
	static constinit u32 count = 0;
	assert(count < maxVal<u32>);
	return TypeId { ++count };
}();

template <typename T>
struct TypeInfo
{
	static constexpr u32 id    = ::id<T>;
	static constexpr u32 align = alignof(T); // TODO: Might need to be signed for mismatches
	static constexpr u64 size  = sizeof(T);  // TODO: Might need to be signed for mismatches

	// TODO: Overloads are awkward here
	using DefaultConstructFn = void(Span<T> dst);
	static constexpr DefaultConstructFn* defaultConstruct = &::defaultConstruct<T>;

	using ValueConstructFn = void(Span<T> dst);
	static constexpr ValueConstructFn* valueConstruct = &::valueConstruct<T>;

	using CopyConstruct1Fn = void(Span<T> dst, const T& src);
	static constexpr CopyConstruct1Fn* copyConstruct1 = &::copyConstruct<T>;

	using CopyConstructFn = void(Span<T> dst, Span<const T> src);
	static constexpr CopyConstructFn* copyConstruct = &::copyConstruct<T>;

	using MoveConstruct1Fn = void(Span<T> dst, T&& src);
	static constexpr MoveConstruct1Fn* moveConstruct1 = &::moveConstruct<T>;

	using MoveConstructFn = void(Span<T> dst, Span<T> src);
	static constexpr MoveConstructFn* moveConstruct = &::moveConstruct<T>;

	using DestructFn = void(Span<T> dst);
	static constexpr DestructFn* destruct = &::destruct<T>;

	using RelocateFn = void(Span<T> dst, Span<T> src);
	static constexpr RelocateFn* relocate = &::relocate<T>;

	using CopyAssign1Fn = void(Span<T> dst, const T& src);
	static constexpr CopyAssign1Fn* copyAssign1 = &::copyAssign<T>;

	using CopyAssignFn = void(Span<T> dst, Span<const T> src);
	static constexpr CopyAssignFn* copyAssign = &::copyAssign<T>;

	using MoveAssign1Fn = void(Span<T> dst, T&& src);
	static constexpr MoveAssign1Fn* moveAssign1 = &::moveAssign<T>;

	using MoveAssignFn = void(Span<T> dst, Span<T> src);
	static constexpr MoveAssignFn* moveAssign = &::moveAssign<T>;
};
