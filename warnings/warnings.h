#pragma GCC diagnostic ignored     "-Wpragmas"

#pragma GCC diagnostic warning     "-Wall"
#pragma GCC diagnostic warning     "-Weffc++"
#pragma GCC diagnostic warning     "-Wextra"

//#pragma GCC diagnostic warning     "-Waggressive-loop-optimizations"
//#pragma GCC diagnostic warning     "-Walloc-zero"
#pragma GCC diagnostic warning     "-Walloca"
//#pragma GCC diagnostic warning     "-Walloca-larger-than=8192"
#pragma GCC diagnostic warning     "-Warray-bounds"
#pragma GCC diagnostic warning     "-Wcast-align"
#pragma GCC diagnostic warning     "-Wcast-qual"
#pragma GCC diagnostic warning     "-Wchar-subscripts"
//#pragma GCC diagnostic warning     "-Wconditionally-supported"
#pragma GCC diagnostic warning     "-Wconversion"
#pragma GCC diagnostic warning     "-Wctor-dtor-privacy"
#pragma GCC diagnostic warning     "-Wdangling-else"
//#pragma GCC diagnostic warning     "-Wduplicated-branches"
#pragma GCC diagnostic warning     "-Wempty-body"
#pragma GCC diagnostic warning     "-Wfloat-equal"
#pragma GCC diagnostic warning     "-Wformat-nonliteral"
//#pragma GCC diagnostic warning     "-Wformat-overflow=2"
#pragma GCC diagnostic warning     "-Wformat-security"
//#pragma GCC diagnostic warning     "-Wformat-signedness"
//#pragma GCC diagnostic warning     "-Wformat-truncation=2"
#pragma GCC diagnostic warning     "-Wformat=2"
//#pragma GCC diagnostic warning     "-Wlarger-than=8192"
//#pragma GCC diagnostic warning     "-Wlogical-op"
#pragma GCC diagnostic warning     "-Wmismatched-tags"
#pragma GCC diagnostic warning     "-Wmissing-declarations"
#pragma GCC diagnostic warning     "-Wnarrowing"
#pragma GCC diagnostic warning     "-Wnon-virtual-dtor"
#pragma GCC diagnostic warning     "-Wnonnull"
//#pragma GCC diagnostic warning     "-Wopenmp-simd"
#pragma GCC diagnostic warning     "-Woverloaded-virtual"
#pragma GCC diagnostic warning     "-Wpacked"
#pragma GCC diagnostic warning     "-Wpointer-arith"
#pragma GCC diagnostic warning     "-Wredundant-decls"
//#pragma GCC diagnostic warning     "-Wredundant-tags"
//#pragma GCC diagnostic warning     "-Wrestrict"
#pragma GCC diagnostic warning     "-Wshadow"
#pragma GCC diagnostic warning     "-Wsign-promo"
//#pragma GCC diagnostic warning     "-Wstack-usage=8192"
#pragma GCC diagnostic warning     "-Wstrict-aliasing"
//#pragma GCC diagnostic warning     "-Wstrict-null-sentinel"
#pragma GCC diagnostic warning     "-Wstrict-overflow=2"
//#pragma GCC diagnostic warning     "-Wstringop-overflow=4"
//#pragma GCC diagnostic warning     "-Wsuggest-attribute=noreturn"
//#pragma GCC diagnostic warning     "-Wsuggest-final-methods"
//#pragma GCC diagnostic warning     "-Wsuggest-final-types"
#pragma GCC diagnostic warning     "-Wsuggest-override"
#pragma GCC diagnostic warning     "-Wswitch-default"
#pragma GCC diagnostic warning     "-Wswitch-enum"
//#pragma GCC diagnostic warning     "-Wsync-nand"
#pragma GCC diagnostic warning     "-Wundef"
#pragma GCC diagnostic warning     "-Wunused"
#pragma GCC diagnostic warning     "-Wvarargs"
//#pragma GCC diagnostic warning     "-Wvla-larger-than=8192"

#pragma GCC diagnostic error       "-Wsizeof-array-argument"

#pragma GCC diagnostic ignored     "-Waddress"
#pragma GCC diagnostic ignored     "-Winline"
//#pragma GCC diagnostic ignored     "-Wliteral-suffix"
#pragma GCC diagnostic ignored     "-Wmissing-field-initializers"
//#pragma GCC diagnostic ignored     "-Wnonnull-compare"
#pragma GCC diagnostic ignored     "-Wold-style-cast"
#pragma GCC diagnostic ignored     "-Wunreachable-code"
#pragma GCC diagnostic ignored     "-Wunused-const-variable"
#pragma GCC diagnostic ignored     "-Wunused-function"
#pragma GCC diagnostic ignored     "-Wvariadic-macros"

#pragma GCC diagnostic warning     "-Wpragmas"

#define     _FORTIFY_SOURCE        2

#ifndef     _GLIBCXX_NDEBUG                  // TXLib enables _GLIBCXX_DEBUG by default. When using third-party libraries
    #define _GLIBCXX_DEBUG                   // compiled without _GLIBCXX_DEBUG (SFML, for example), #define _GLIBCXX_NDEBUG
    #define _GLIBCXX_DEBUG_PEDANTIC          // *before* including TXLib.h.
#endif
