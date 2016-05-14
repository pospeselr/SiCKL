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
