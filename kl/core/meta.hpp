#pragma once

#include "kl/containers/span.hpp"
#include "kl/core/compiler.hpp"
#include "kl/core/hash.hpp"
#include "kl/memory/lifetime.hpp"

// TODO: Rename to type-info?

namespace kl
{
	// TODO: Operators? Base class for strongly typed integers?
	struct TypeId
	{
		u64 val;
	};

	// TODO: Replace this with build system generated data. Hashing at compile time is slow (best
	// case 170 us per id on msvc) and this approach isn't stable across compilers.
	template <typename T>
	constexpr TypeId _typeId()
	{
		#if KL_COMPILER_MSVC || KL_COMPILER_CLANG_CL
		// NOTE: Includes the null terminator but that's ok
		u8 name[] = { __FUNCSIG__ };
		#else
		constexpr Span<const c8> chars = __PRETTY_FUNCTION__;
		u8 name[chars.len];
		convertAssign<u8>(name, chars);
		#endif

		u64 hash = hashFnv1a(name);
		return TypeId { hash };
	}

	template <typename T>
	constexpr TypeId id = _typeId<T>();

	template <typename T>
	struct TypeInfo
	{
		static constexpr TypeId id    = kl::id<T>;
		static constexpr u32    align = alignof(T);
		static constexpr u64    size  = sizeof(T);

		// TODO: Overloads are awkward here
		using DefaultConstructFn = void(Span<T> dst);
		static constexpr DefaultConstructFn* defaultConstruct = &kl::defaultConstruct<T>;

		using ValueConstructFn = void(Span<T> dst);
		static constexpr ValueConstructFn* valueConstruct = &kl::valueConstruct<T>;

		using CopyConstruct1Fn = void(Span<T> dst, const T& src);
		static constexpr CopyConstruct1Fn* copyConstruct1 = &kl::copyConstruct<T>;

		using CopyConstructFn = void(Span<T> dst, Span<const T> src);
		static constexpr CopyConstructFn* copyConstruct = &kl::copyConstruct<T>;

		using MoveConstruct1Fn = void(Span<T> dst, T&& src);
		static constexpr MoveConstruct1Fn* moveConstruct1 = &kl::moveConstruct<T>;

		using MoveConstructFn = void(Span<T> dst, MoveSpan<T> src);
		static constexpr MoveConstructFn* moveConstruct = &kl::moveConstruct<T>;

		using DestructFn = void(Span<T> dst);
		static constexpr DestructFn* destruct = &kl::destruct<T>;

		using RelocateFn = void(Span<T> dst, MoveSpan<T> src);
		static constexpr RelocateFn* relocate = &kl::relocate<T>;

		using CopyAssign1Fn = void(Span<T> dst, const T& src);
		static constexpr CopyAssign1Fn* copyAssign1 = &kl::copyAssign<T>;

		using CopyAssignFn = void(Span<T> dst, Span<const T> src);
		static constexpr CopyAssignFn* copyAssign = &kl::copyAssign<T>;

		using MoveAssign1Fn = void(Span<T> dst, T&& src);
		static constexpr MoveAssign1Fn* moveAssign1 = &kl::moveAssign<T>;

		using MoveAssignFn = void(Span<T> dst, MoveSpan<T> src);
		static constexpr MoveAssignFn* moveAssign = &kl::moveAssign<T>;
	};
}

// -------------------------------------------------------------------------------------------------
// Implementation

namespace kl
{
	constexpr TypeId id_i64 = id<i64>;
}
