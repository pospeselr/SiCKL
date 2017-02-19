#pragma once

// platform/compiler detection

#ifdef _MSC_VER
    #define COMPILER_MSVC
#endif

#ifdef __clang__
    #define COMPILER_CLANG
#endif

#ifdef __GNUC__
    #define COMPILER_GCC
#endif