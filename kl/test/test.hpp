#pragma once

#include "kl/containers/string-view.hpp"
#include "kl/debug/source-location.hpp"
#include "kl/temp/print.hpp"

// -------------------------------------------------------------------------------------------------
// Interface

namespace kl
{
	struct Test;

	struct TestContext
	{
		const Test* test;
		b8          failed;
	};

	using RunFn = void(TestContext& ctx);

	struct Test
	{
		StringView name;
		RunFn*     run;
	};

	struct RunTest
	{
		RunTest(CStringView name, RunFn* run);
	};

	i32 runTests();
	b8 check(TestContext& ctx, b8 condition, SourceLocation location = {});

	template <typename T> b8 check(TestContext& ctx, const T& condition, SourceLocation location = {});
	template <typename A, typename B> b8 checkEq(TestContext& ctx, const A& actual, const B& expected, SourceLocation location = {});
	template <typename A, typename B> b8 checkNe(TestContext& ctx, const A& actual, const B& expected, SourceLocation location = {});
	template <typename A, typename B> b8 checkLt(TestContext& ctx, const A& actual, const B& expected, SourceLocation location = {});
	template <typename A, typename B> b8 checkLe(TestContext& ctx, const A& actual, const B& expected, SourceLocation location = {});
	template <typename A, typename B> b8 checkGt(TestContext& ctx, const A& actual, const B& expected, SourceLocation location = {});
	template <typename A, typename B> b8 checkGe(TestContext& ctx, const A& actual, const B& expected, SourceLocation location = {});
}

// -------------------------------------------------------------------------------------------------
// Implementation

namespace kl
{
	template <typename T>
	b8
	check(TestContext& ctx, const T& condition, SourceLocation location)
	{
		return check(ctx, b8(condition), location);
	}

	template <typename A, typename B>
	b8
	checkEq(TestContext& ctx, const A& actual, const B& expected, SourceLocation location)
	{
		b8 passed = check(ctx, actual == expected, location);
		if (!passed)
		{
			print("\tActual:   ", actual,   "\n");
			print("\tExpected: ", expected, "\n");
		}
		return passed;
	}

	template <typename A, typename B>
	b8
	checkNe(TestContext& ctx, const A& actual, const B& expected, SourceLocation location)
	{
		b8 passed = check(ctx, actual != expected, location);
		if (!passed)
		{
			print("\tActual:      ", actual,   "\n");
			print("\tExpected: != ", expected, "\n");
		}
		return passed;
	}

	template <typename A, typename B>
	b8
	checkLt(TestContext& ctx, const A& actual, const B& expected, SourceLocation location)
	{
		b8 passed = check(ctx, actual < expected, location);
		if (!passed)
		{
			print("\tActual:     ", actual,   "\n");
			print("\tExpected: < ", expected, "\n");
		}
		return passed;
	}

	template <typename A, typename B>
	b8
	checkLe(TestContext& ctx, const A& actual, const B& expected, SourceLocation location)
	{
		b8 passed = check(ctx, actual <= expected, location);
		if (!passed)
		{
			print("\tActual:      ", actual,   "\n");
			print("\tExpected: <= ", expected, "\n");
		}
		return passed;
	}

	template <typename A, typename B>
	b8
	checkGt(TestContext& ctx, const A& actual, const B& expected, SourceLocation location)
	{
		b8 passed = check(ctx, actual > expected, location);
		if (!passed)
		{
			print("\tActual:     ", actual,   "\n");
			print("\tExpected: > ", expected, "\n");
		}
		return passed;
	}

	template <typename A, typename B>
	b8
	checkGe(TestContext& ctx, const A& actual, const B& expected, SourceLocation location)
	{
		b8 passed = check(ctx, actual >= expected, location);
		if (!passed)
		{
			print("\tActual:      ", actual,   "\n");
			print("\tExpected: >= ", expected, "\n");
		}
		return passed;
	}
}
