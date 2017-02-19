#pragma once

#ifdef COMPILER_MSVC
    #include <intrin.h>
    #define DEBUGBREAK() __debugbreak()
#endif

#ifdef COMPILER_CLANG
    #define DEBUGBREAK() __builtin_trap()
#endif

#ifdef COMPILER_GCC
    #define DEBUGBREAK() raise(SIGTRAP)
#endif

#define UNREFERENCED_PARAMETER(X) (void)X;
#define TRACE printf("%u: %s\n", __LINE__, __PRETTY_FUNCTION__);

// countof(X)
template<typename T, size_t N>
constexpr size_t countof(const T (&)[N]) { return N; }
