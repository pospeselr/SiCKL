#pragma once

// platform/compiler detection

#if defined(_MSC_VER)
    #define SPARK_COMPILER_MSVC
#elif defined(__clang__)
    #define SPARK_COMPILER_CLANG
#elif defined(__GNUC__)
    #define SPARK_COMPILER_GCC
#else
    #error "unknown compiler"
#endif

// break if in debugger
#if defined(SPARK_COMPILER_MSVC)
    #include <intrin.h>
    #define SPARK_DEBUGBREAK() __debugbreak()
#elif defined(SPARK_COMPILER_CLANG)
    #define SPARK_DEBUGBREAK() __builtin_trap()
#elif defined(SPARK_COMPILER_GCC)
    #define SPARK_DEBUGBREAK() raise(SIGTRAP)
#endif

// export functions from shared library/dll
#if defined(SPARK_COMPILER_MSVC)
    #define SPARK_EXPORT extern "C" __declspec(dllexport)
#elif defined(SPARK_COMPILER_CLANG)
    #define SPARK_EXPORT extern "C" __attribute__ ((visibility ("default")))
#elif defined(SPARK_COMPILER_GCC)
    #define SPARK_EXPORT extern "C" __attribute__ ((visibility ("default")))
#endif

// force inline of function
#if defined(SPARK_COMPILER_MSVC)
    #define SPARK_FORCE_INLINE __declspec(__forceinline)
#elif defined(SPARK_COMPILER_CLANG)
    #define SPARK_FORCE_INLINE __attribute__ ((always_inline)) inline
#elif defined(SPARK_COMPILER_GCC)
    #define SPARK_FORCE_INLINE __attribute__ ((always_inline)) inline
#endif

// force never inline of function
#if defined(SPARK_COMPILER_MSVC)
    #define SPARK_NEVER_INLINE __declspec(noinline)
#elif defined(SPARK_COMPILER_CLANG)
    #define SPARK_NEVER_INLINE __attribute__ ((noinline))
#elif defined(SPARK_COMPILER_GCC)
    #define SPARK_NEVER_INLINE __attribute__ ((noinline))
#endif