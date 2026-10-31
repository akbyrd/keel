#pragma once

#include "kl/containers/span.hpp"
#include "kl/core/compiler.hpp"
#include "kl/core/hash.hpp"
#include "kl/core/math.hpp"
#include "kl/memory/lifetime.hpp"

namespace kl
{
	// TODO: Operators? Base class for strongly typed integers?
	struct TypeId
	{
		u64 val;
	};

	// TODO: Replace with build system generated ids that are guaranteed unique.
	template <typename T>
	constexpr TypeId _typeId()
	{
		constexpr const c8* sig = KL_FUNCTION_SIGNATURE;
		constexpr i64       len = i64(__builtin_strlen(sig));

		// NOTE: Copy to bytes because reinterpret_cast is not allowed in constant evaluation
		u8 bytes[len] = {};
		for (i64 i = 0; i < len; ++i)
			bytes[i] = u8(sig[i]);

		return TypeId { hashFnv1a(Span<const u8> { len, bytes }) };
	}

	template <typename T>
	constexpr TypeId id = _typeId<T>();

	template <typename T>
	struct TypeInfo
	{
		static constexpr TypeId id    = kl::id<T>;
		static constexpr u32    align = alignof(T); // TODO: Might need to be signed for mismatches
		static constexpr u64    size  = sizeof(T);  // TODO: Might need to be signed for mismatches

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

		using MoveConstructFn = void(Span<T> dst, Span<T> src);
		static constexpr MoveConstructFn* moveConstruct = &kl::moveConstruct<T>;

		using DestructFn = void(Span<T> dst);
		static constexpr DestructFn* destruct = &kl::destruct<T>;

		using RelocateFn = void(Span<T> dst, Span<T> src);
		static constexpr RelocateFn* relocate = &kl::relocate<T>;

		using CopyAssign1Fn = void(Span<T> dst, const T& src);
		static constexpr CopyAssign1Fn* copyAssign1 = &kl::copyAssign<T>;

		using CopyAssignFn = void(Span<T> dst, Span<const T> src);
		static constexpr CopyAssignFn* copyAssign = &kl::copyAssign<T>;

		using MoveAssign1Fn = void(Span<T> dst, T&& src);
		static constexpr MoveAssign1Fn* moveAssign1 = &kl::moveAssign<T>;

		using MoveAssignFn = void(Span<T> dst, Span<T> src);
		static constexpr MoveAssignFn* moveAssign = &kl::moveAssign<T>;
	};
}
