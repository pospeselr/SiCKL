#pragma once

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

#define SICKL_ASSERT(X) do { if(!(X)) { spark_print_assert(#X, __FILE__, (uint32_t)__LINE__); } } while(0)

// error handling
extern "C" void spark_print_assert(const char* msg, const char* file, uint32_t line);
extern "C" void spark_print_exception(const std::exception& ex);
