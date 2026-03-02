MSVC
/W3 /WX /analyze
/W4?
Don't use /Wall
/external
/fsanitize
/d1reportTime
/d1reportAllClassLayout
https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warnings-c4000-c5999
https://learn.microsoft.com/en-us/cpp/preprocessor/compiler-warnings-that-are-off-by-default

Clang
-Wall -Wextra -Werror -Wshadow -Wconversion
Don't use -Weverything
--analyze
-ftime-trace
-fdump-record-layout
https://clang.llvm.org/docs/DiagnosticsReference.html
https://github.com/llvm/llvm-project/blob/main/clang/include/clang/Basic/Builtins.td

GCC
-Wall -Wextra -Werror -Wshadow -Wconversion
-fsanitize (memory, undefined, leak, thread, fuzzer)
https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
https://kristerw.blogspot.com/2017/09/useful-gcc-warning-options-not-enabled.html

TODO
Disable ASLR
Debug levels / profiles
valgrind / cachegrind
per-file options (push/pop profile macro?)
libFuzzer
