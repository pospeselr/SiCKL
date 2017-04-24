#pragma once

// platform/compiler detection

#if defined(_MSC_VER)
    #define RUFF_COMPILER_MSVC
#elif defined(__clang__)
    #define RUFF_COMPILER_CLANG
#elif defined(__GNUC__)
    #define RUFF_COMPILER_GCC
#else
    #error "unknown compiler"
#endif

// break if in debugger
#if defined(RUFF_COMPILER_MSVC)
    #include <intrin.h>
    #define RUFF_DEBUGBREAK() __debugbreak()
#elif defined(RUFF_COMPILER_CLANG)
    #define RUFF_DEBUGBREAK() __builtin_trap()
#elif defined(RUFF_COMPILER_GCC)
    #include <signal.h>
    #define RUFF_DEBUGBREAK() raise(SIGTRAP)
#endif

// export functions from shared library/dll
#if defined(RUFF_COMPILER_MSVC)
    #define RUFF_EXPORT extern "C" __declspec(dllexport)
#elif defined(RUFF_COMPILER_CLANG)
    #define RUFF_EXPORT extern "C" __attribute__ ((visibility ("default")))
#elif defined(RUFF_COMPILER_GCC)
    #define RUFF_EXPORT extern "C" __attribute__ ((visibility ("default")))
#endif

// force inline of function
#if defined(RUFF_COMPILER_MSVC)
    #define RUFF_FORCE_INLINE __declspec(__forceinline)
#elif defined(RUFF_COMPILER_CLANG)
    #define RUFF_FORCE_INLINE __attribute__ ((always_inline)) inline
#elif defined(RUFF_COMPILER_GCC)
    #define RUFF_FORCE_INLINE __attribute__ ((always_inline)) inline
#endif

// force never inline of function
#if defined(RUFF_COMPILER_MSVC)
    #define RUFF_NEVER_INLINE __declspec(noinline)
#elif defined(RUFF_COMPILER_CLANG)
    #define RUFF_NEVER_INLINE __attribute__ ((noinline))
#elif defined(RUFF_COMPILER_GCC)
    #define RUFF_NEVER_INLINE __attribute__ ((noinline))
#endif