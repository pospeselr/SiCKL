#pragma once

// std
#include <csignal>

#ifdef Compiler_MSVC
    #include <intrin.h>
    #define DEBUGBREAK() __debugbreak()
#endif
#ifdef Compiler_Clang
    #define DEBUGBREAK() __builtin_trap()
#endif
#ifdef Compiler_GCC
    #define DEBUGBREAK() raise(SIGTRAP)
#endif

#ifdef SPARK_DEBUG
	#define SPARK_ASSERT(X) do { if(!(X)) { spark_print_assert(#X, __FILE__, (uint32_t)__LINE__); } } while(0)
	#define SPARK_VERIFY(X) SPARK_ASSERT(X)
#else
	#define SPARK_ASSERT(X) (void)0
	#define SPARK_VERIFY(X) (void)(X)
#endif

#define SPARK_STATIC_ASSERT(...) static_assert((__VA_ARGS__), #__VA_ARGS__)

// error handling
extern "C" void spark_print_assert(const char* msg, const char* file, uint32_t line);
extern "C" void spark_print_exception(const std::exception& ex);
