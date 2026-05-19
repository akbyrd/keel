# TODO
* Compile Time Optimization
	* Option 1 - Implementations in .inl files
	* Option 2 - Unity build / merge .cpp files
	* Option 3 - Pre-compiled headers
	* Option 4 - C++ modules (probably this?)
* Organization
	* What's the policy for hpp vs cpp?
	* No standard library headers
	* No platform headers
	* Support file nesting in common editors
	* Automatic formatting
	* Tests that verify the interface + implementation split



# Build Configuration
## TODO
Disable ASLR
Debug levels / profiles
valgrind / cachegrind
per-file options (push/pop profile macro?)
libFuzzer

## Language
C++ 20
PDF:  https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/n4861.pdf
HTML: https://timsong-cpp.github.io/cppwp/n4861/

## MSVC
/W3 /WX /analyze
/W4?
Don't use /Wall
/external
/fsanitize
/d1reportTime
/d1reportAllClassLayout
https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warnings-c4000-c5999
https://learn.microsoft.com/en-us/cpp/preprocessor/compiler-warnings-that-are-off-by-default
https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros

## Clang
-Wall -Wextra -Werror -Wshadow -Wconversion
Don't use -Weverything
--analyze
-ftime-trace
-fdump-record-layout
https://clang.llvm.org/docs/DiagnosticsReference.html
https://github.com/llvm/llvm-project/blob/main/clang/include/clang/Basic/Builtins.td
Print predefined macros
```
"" | clang -x c -dM -E -
```

## GCC
-Wall -Wextra -Werror -Wshadow -Wconversion
-fsanitize (memory, undefined, leak, thread, fuzzer)
https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html



# Documentation
## TODO
* Which diagram formats make sense?
* Choose a documentation processing tool (Doxygen, MrDocs, clang-doc, or custom).

## Requirements
* The simplest tool that solves the problem well.
* Documentation is written and versioned with the actual source code.
* Documentation can be parsed by a tool to generate other formats, such as HTML.
* Documentation lives on declarations, not definitions.
* Long form prose can be written, both at the file-level and the directory level, perhaps in Markdown.
* References in documentation is "type checked" to ensure validity.
* Handles C++ features such as constraints, contracts, templates, and attributes.
* Works with common code editors. Ideally all of vscode, Zed, Visual Studio, and Rider.
* Support for diagrams of some sort.

### Nice-to-haves
* Works like a compiler. Can be invoked for a single file. Can perform incremental rebuilds based on file changes.

## Format
All current options support Doxygen style, so that is the format we use. If all tools end up being inadequate we can write our own but continue to use the Doxygen format.

## Options
### Doxygen
* https://www.doxygen.nl/manual/commands.html
* May not handle all C++ features (e.g. constraints, attributes, contracts, etc).

### MrDocs
* https://www.mrdocs.com/
* Explicitly not a compiler, but uses a compilation database.
	* https://cppalliance.org/alan/2026/04/24/Alan.html#why-we-discarded-mrdocs-as-compiler
* Full time maintenance
* Explicitly uses Doxygen style
	* https://www.mrdocs.com/docs/mrdocs/commands.html
	* https://docs.oracle.com/en/java/javase/13/docs/specs/javadoc/doc-comment-spec.html
* Appears to require a compilation database
* Appears not to create documentation for headers unless they are included

### clang-doc
* https://clang.llvm.org/extra/clang-doc.html
* Nascent
* Acts like a compiler
* Can use a compilation database, but not required
* Supports Doxygen style
* Supports yaml, md, html, and json output
