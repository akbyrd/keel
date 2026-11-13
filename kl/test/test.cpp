#include "kl/containers/span.hpp"
#include "kl/debug/assert.hpp"
#include "kl/temp/print.hpp"
#include "kl/test/test.hpp"

namespace kl
{
	namespace
	{
		constinit Test       testStorage[4096] = {};
		constinit Span<Test> tests             = { 0, testStorage };

		b8
		isSeparator(c8 x)
		{
			return x == '/' || x == '\\';
		}

		void
		printPath(CStringView file)
		{
			i64 start = 0;
			for (i64 i = 0; i + 2 < file.len; i++)
			{
				b8 isComponent = i == 0 || isSeparator(file[i - 1]);
				if (isComponent && file[i] == 'k' && file[i + 1] == 'l' && isSeparator(file[i + 2]))
					start = i + 3;
			}

			for (i64 i = start; i < file.len; i++)
				print(isSeparator(file[i]) ? '/' : file[i]);
		}
	}

	RunTest::RunTest(CStringView name, RunFn* run)
	{
		assert(tests.len < extent<decltype(testStorage)>);
		tests.data[tests.len] = { StringView(name), run };
		tests.len++;
	}

	b8
	check(TestContext& ctx, b8 condition, SourceLocation location)
	{
		if (!condition)
		{
			CStringView failMarker = "\xE2\x9D\x8C"; // ❌, U+274C

			ctx.failed = true;
			print("\n", failMarker, " ", ctx.test->name, " - ");
			printPath(location.file);
			print("(", location.line, ")\n");
		}
		return condition;
	}

	i32
	runTests()
	{
		u32 codePage    = beginUtf8Output();
		i64 failedTests = 0;
		for (const Test& test : tests)
		{
			TestContext ctx = { &test, false };
			test.run(ctx);
			if (ctx.failed)
				failedTests++;
		}

		if (failedTests > 0)
			print("\n");
		print(tests.len, " tests, ", failedTests, " failed\n");

		endUtf8Output(codePage);
		return failedTests == 0 ? 0 : 1;
	}
}
