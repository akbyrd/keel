# Guiding Principles
The most important guiding principles by far are readability and discoverability.



# Naming Convention
* Files           - Kebab case  - string-view.hpp
* Primitive Types - Lower case  - c8
* Compound Types  - Pascal case - StringView
* Variables       - Camel case  - strView
* Functions       - Camel case  - toLower

It's important that types and variables do not use the same convention to avoid name clashes, which would require a prefix to disambiguate (`File file = ...;` vs `FFile File = ...;`).

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



# Documentation
* Every function must be documented.
* Explicitly document all assumptions and limitations.
* Use [Doxygen comment syntax](https://www.doxygen.nl/manual/commands.html)
	* Use `///`, not `/** */`
	* Use `@tag`, not `\tag`
	* Use ` ```cpp <code>``` `, not `\code <code> \endcode`
	* Documentation goes on declarations, not definitions
	* Use `@file` comments where useful
	* Use these special files when useful:
		* style.md - unique, top level only - Describes the coding style for the entire codebase.
		* docs.md  - per-directory          - Documents the design and behavior of a system. Focuses user-facing details.
		* notes.md - per-directory          - Documents the rationale and implementation of a system. Focuses on implementation details.



# Headers
Single block of headers, sorted alphabetically. No groups.
```cpp
// foo.cpp

#include "core/bar.hpp"
#include "core/baz.hpp"
#include "core/foo.hpp"
#include <ext/dependency.hpp>
#include <cstdint>
```



# Indices
Indices are signed. This is to avoid unnecessary conversions and reduce the potential for mistakes. No special consideration is needed for reverse loops or subtraction.

Indices are 64 bits. For types that contain a length and a pointer like spans the padding next to a 32-bit index would result in the same total footprint. Types that contain a length, capacity, and pointer will end up 64 bits larger. 32-bit platforms will take a larger hit, but I estimate <5% of relevant CPUs in use in 2026 are 32-bit.



# Const
Use logical const, not physical const. Logical const means no mutations may occur anywhere when arguments are const. Functions on const objects are expected to be thread safe. This means const must be treated as transitive, even though it's not enforced by the language. If a const pointer is passed to a function you may not use that pointer to get to a non-const object and mutate it.

`const_cast` and `mutable` are not allowed. Ever.
```cpp
struct Foo
{
	void Do() const;

	void Do()
	{
		const Foo* ConstThis = this;
		ConstThis->Do();
	}
};
```


# Other
* Tabs for indentation. Spaces for vertical alignment.
* Vertical alignment is encouraged when it improves readability.
* clang-tidy entire file when saving.
* Zero or one function call per line.
* 120 column soft limit.
* Single trailing newline in files.
* `void*` for "unknown type" and `u8*` for bytes and pointer arithmetic.
* Separators are encouraged, but keep them simple and consistent.
	```cpp
	// ------------------ (up to column limit)
	// Implementation
	```
* Wrap each function argument onto a new line when they don't fit on one line.
	```cpp
	void Foo(
		One one,
		Two two,
		Three three,
		...
	)
	```
