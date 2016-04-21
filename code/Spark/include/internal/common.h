#ifndef SPARK_COMMON_H
#define SPARK_COMMON_H

#ifdef _MSC_VER
#   define Compiler_MSVC
#endif

#ifdef __clang__
#   define Compiler_Clang
#endif

#ifdef __GNUC__
#   define Compiler_GCC
#endif

#ifdef Compiler_MSVC
    #include <intrin.h>
    #define DEBUGBREAK() __debugbreak()
#endif
#ifdef Compiler_Clang
    #define DEBUGBREAK() __builtin_trap()
#endif
#ifdef Compiler_GCC
    #define DEBUGBREAK() __builtin_trap()
#endif

#define UNREFERENCED_PARAMETER(X) (void)X;
#define TRACE printf("%u: %s\n", __LINE__, __PRETTY_FUNCTION__);

#endif