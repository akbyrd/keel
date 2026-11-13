#pragma once

#include "kl/containers/string-view.hpp"

namespace kl
{
	struct SourceLocation
	{
		CStringView file = CStringView(__builtin_FILE());
		i32         line = __builtin_LINE();
	};
}
