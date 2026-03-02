# Guiding Principles
The most important guiding principles by far are readability and discoverability.



# Naming Convention
* Files           - Kebab case  - string-view.hpp
* Primitive Types - Lower case  - c8
* Compound Types  - Pascal case - StringView
* Variables       - Camel case  - strView
* Functions       - Camel case  - toLower

It's important that types and variables do not use the same convention (`File file = ...;`) to avoid name clashes (`File File = ...;`) which would require a prefix to disambiguate (`FFile File = ...;`).

Do not use Hungarian notation (`b8 bIsDirty`) or prefixes (`m_member`, `kConstant`, `gGlobal`).



# Source File Organization
* Public Interface      - foo.hpp
* Inline impl           - foo.hpp
* Private impl          - foo.cpp
* Tests                 - foo.cpp
* Platform inline impl  - foo.win32.hpp
* Platform private impl - foo.win32.cpp
* Platform tests        - foo.win32.cpp

## Intent
Keep the number of files reasonably low. Having many small files is bad for build performance. It's also bad for humans navigating and discovering code.

Massive files can be bad for iteration, but this problem is less common and is easier to detect and fix.



# Headers
Single blocks of headers, sorted alphabetically. No groups.
```
// foo.cpp

#include "core/bar.hpp"
#include "core/baz.hpp"
#include "core/foo.hpp"
#include <cstdint>
```



# Other
* Tabs for indentation. Spaces for vertical alignment.
* clang-tidy entire file when saving.
* Zero or one function call per line.
* 120 column soft limit.
* Single trailing newline in files.
* Vertical alignment is encouraged when it improves readability.
* `void*` for "unknown type" and `u8*` for bytes and pointer arithmetic.
* Separators are encouraged.
	```
	// ------------------ (up to column limit)
	// Implementation
	```
* Wrap each function argument onto a new line when they don't fit on one line.
	```
	void Foo(
		One one,
		Two two,
		Three three,
		...
	)
	```
