#pragma once

#ifdef _MSC_VER
	#define Compiler_MSVC
#endif

#ifdef __clang__
	#define Compiler_Clang
#endif

#ifdef __GNUC__
	#define Compiler_GCC
#endif

#ifdef QT_NO_DEBUG
	#define SPARK_RELEASE
#else
	#define SPARK_DEBUG
#endif

#define UNREFERENCED_PARAMETER(X) (void)X;
#define TRACE printf("%u: %s\n", __LINE__, __PRETTY_FUNCTION__);

// countof(X)
#ifdef Compiler_MSVC
	#define countof(X) _countof(X)
#else
	template<typename T, size_t N>
	constexpr size_t countof(const T (&)[N]) { return N; }
#endif

namespace Spark
{
    // scope exit
    struct scope_exit
    {
        scope_exit(std::function<void()>&& func) : _func(func) {}
        ~scope_exit()
        {
            _func();
        }
    private:
        std::function<void(void)> _func;
    };
}