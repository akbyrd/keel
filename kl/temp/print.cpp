#include "kl/core/platform.hpp"
#include "kl/temp/print.hpp"
#include <stdio.h>

#if KL_PLATFORM_WIN32
#include "kl/core/win32.hpp"
#endif

namespace kl
{
	u32
	beginUtf8Output()
	{
		#if KL_PLATFORM_WIN32
		u32 codePage = GetConsoleOutputCP();
		SetConsoleOutputCP(CP_UTF8);
		return codePage;
		#else
		return 0;
		#endif
	}

	void
	endUtf8Output(u32 codePage)
	{
		fflush(stdout);
		#if KL_PLATFORM_WIN32
		if (codePage != 0)
			SetConsoleOutputCP(codePage);
		#else
		(void) codePage;
		#endif
	}

	void
	print(StringView x)
	{
		fwrite(x.data, 1, u64(x.len), stdout);
	}

	void
	print(CStringView x)
	{
		print(StringView(x));
	}

	void
	print(b8 x)
	{
		fputs(x ? "true" : "false", stdout);
	}

	void
	print(c8 x)
	{
		fputc(x, stdout);
	}

	void
	print(i64 x)
	{
		printf("%lld", static_cast<long long>(x));
	}

	void
	print(u64 x)
	{
		printf("%llu", static_cast<unsigned long long>(x));
	}

	void
	print(f64 x)
	{
		printf("%.17g", x);
	}

	void
	print(const void* x)
	{
		printf("%p", x);
	}
}
