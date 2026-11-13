#pragma once

#include "kl/containers/span.hpp"
#include "kl/test/test.hpp"

namespace kl::test::span
{
	inline RunTest
	zeroInit("zeroInit", [](TestContext& ctx)
	{
		Span<i32> span = {};
		checkEq(ctx, span.len, 0);
		checkEq(ctx, span.data, nullptr);
		check(ctx, !span);
	});

	inline RunTest
	fromSingle("fromSingle", [](TestContext& ctx)
	{
		i32 value = 7;
		Span<i32> span = value;
		checkEq(ctx, span.len, 1);
		checkEq(ctx, span.data, &value);
	});

	inline RunTest
	fromLenAndData("fromLenAndData", [](TestContext& ctx)
	{
		i32 values[] = { 1, 2, 3 };
		Span<i32> span = { 2, values + 1 };
		checkEq(ctx, span.len, 2);
		checkEq(ctx, span.data, values + 1);
	});

	inline RunTest
	fromArray("fromArray", [](TestContext& ctx)
	{
		i32 values[] = { 1, 2, 3 };
		Span<i32> span = values;
		checkEq(ctx, span.len, 3);
		checkEq(ctx, span.data, values);
		check(ctx, span);
	});

	inline RunTest
	index("index", [](TestContext& ctx)
	{
		i32 values[] = { 1, 2, 3 };
		Span<i32> span = values;
		checkEq(ctx, span[0], 1);
		checkEq(ctx, span[2], 3);

		span[1] = 5;
		checkEq(ctx, values[1], 5);
	});

	inline RunTest
	toConst("toConst", [](TestContext& ctx)
	{
		i32 values[] = { 1, 2, 3 };
		Span<i32> span = values;
		Span<const i32> constSpan = span;
		checkEq(ctx, constSpan.len, 3);
		checkEq(ctx, constSpan.data, values);
	});

	inline RunTest
	iterate("iterate", [](TestContext& ctx)
	{
		i32 values[] = { 1, 2, 3 };
		Span<i32> span = values;
		checkEq(ctx, span.begin(), values);
		checkEq(ctx, span.end(), values + 3);

		i32 sum = 0;
		for (i32 x : span) sum += x;
		checkEq(ctx, sum, 6);

		Span<const i32> constSpan = values;
		checkEq(ctx, constSpan[1], 2);
		checkEq(ctx, constSpan.begin(), values);
		checkEq(ctx, constSpan.end(), values + 3);
	});

	inline RunTest
	resize("resize", [](TestContext& ctx)
	{
		i32 values[] = { 1, 2, 3, 4 };
		Span<i32> span = { 2, values + 1 };

		Span<i32> grown = span.resize(-1, 1);
		checkEq(ctx, grown.len, 4);
		checkEq(ctx, grown.data, values);

		Span<i32> front = span.resizeFront(1);
		checkEq(ctx, front.len, 1);
		checkEq(ctx, front.data, values + 2);

		Span<i32> back = span.resizeBack(-1);
		checkEq(ctx, back.len, 1);
		checkEq(ctx, back.data, values + 1);
	});

	inline RunTest
	nextPrev("nextPrev", [](TestContext& ctx)
	{
		i32 values[] = { 1, 2, 3, 4, 5 };
		Span<i32> span = { 2, values + 1 };

		Span<i32> next = span.next(2);
		checkEq(ctx, next.len, 2);
		checkEq(ctx, next.data, values + 3);

		Span<i32> prev = span.prev(1);
		checkEq(ctx, prev.len, 1);
		checkEq(ctx, prev.data, values);
	});

	inline RunTest
	overlaps("overlaps", [](TestContext& ctx)
	{
		i32 values[] = { 1, 2, 3, 4 };
		i32 others[] = { 5, 6 };
		Span<i32> span = { 2, values };

		check(ctx, span.overlaps(Span<const i32> { 2, values + 1 }));
		check(ctx, !span.overlaps(Span<const i32> { 2, values + 2 }));
		check(ctx, !span.overlaps(Span<const i32> { 2, others }));
	});

	inline RunTest
	overlapsOtherType("overlapsOtherType", [](TestContext& ctx)
	{
		i32 values[] = { 1, 2, 3, 4 };
		Span<i32> span = { 2, values };
		Span<const u8> bytes = Span<i32>(values).asBytes();

		check(ctx, span.overlaps(bytes.resizeFront(4)));
		check(ctx, !span.overlaps(bytes.resizeFront(8)));
	});

	inline RunTest
	asBytes("asBytes", [](TestContext& ctx)
	{
		i32 values[] = { 1, 2, 3 };
		Span<i32> span = values;
		Span<u8> bytes = span.asBytes();
		checkEq(ctx, bytes.len, 12);
		checkEq(ctx, bytes.data, reinterpret_cast<u8*>(values));
	});

	inline RunTest
	as("as", [](TestContext& ctx)
	{
		i32 values[] = { 1, 2, 3 };
		Span<u8> bytes = Span<i32>(values).asBytes();
		Span<i32> span = bytes.as<i32>();
		checkEq(ctx, span.len, 3);
		checkEq(ctx, span.data, values);
	});
}
