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
	* How are documentation comments handled with this split?
	* Parse and generate documentation
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
