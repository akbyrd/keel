#pragma once

#include "kl/containers/span.hpp"

namespace kl
{
	// TODO:
	//                   Alloc Null String
	// CString         - Yes   Yes  Yes
	// CStringView     - No    Yes  Yes
	// String          - Yes   No   Yes
	// StringView      - No    No   Yes
	// Array<const c8> - Yes   No   No
	// Span<const c8>  - No    No   No

	// TODO: Add non-const versions
	// TODO: Consider string views as specializations of spans
	// TODO: What about StringView<c8>, StringView<utf8>, etc?)
	// TODO: Naming: slice vs span vs view

	// NOTE: Type categories
	// Standard Layout - C memory layout
	// Trivial         - Trivial ctor, dtor, copy, move
	// POD             - Trivial + Standard Layout, is Aggregate, no virtual, no base
	// Aggregate       - public base, member initializers

	// Type              | Recursive | Base   | Virtual | Access | ctor/dtor | move/copy | Member Init | Ref Member |
	// Trivial           |           | Yes    | No      | ----   | Trivial   | Trivial   |             |            | Removed in 26
	// Standard Layout   | Yes       | Yes    | No      | Same   | Trivial   | Trivial   |             |            |
	// Aggregate         | ----      | Public | No      | Public | ----      | ----      | Yes         |            |
	// Implicit Lifetime | ----      | ----   | ----    | ----   | Trivial   | ----      |             | ----       |
	//
	// POD: Trivial + Standard Layout + Aggregate, recursive, deprecated

	// NOTE: Private inheritance
	// Con: Braced-init doesn't work in derived class. Need manual constructors. Lose aggregate status.

	struct StringView
	{
		i64       len;
		const c8* data;

		constexpr operator Span<const c8>() const { return { len, data }; }

		constexpr const c8& operator[](i64 x) const { return data[x]; }
	};

	struct CStringView
	{
		i64       len;
		const c8* data;

		CStringView() = default;

		constexpr explicit CStringView(const c8* x)
			: len(__builtin_strlen(x))
			, data(x)
		{}

		constexpr operator StringView() const { return { len, data }; }
		constexpr operator Span<const c8>() const { return { len, data }; }

		constexpr const c8& operator[](i64 x) const { return data[x]; }
	};
}
