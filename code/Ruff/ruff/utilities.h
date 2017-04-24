#pragma once

#include <stdio.h>
#define RUFF_TRACE() printf("%s(%u) : %s\n", __FILE__, (uint32_t)__LINE__, __PRETTY_FUNCTION__)

#define RUFF_UNREFERENCED_PARAMETER(X) (void)(X);

namespace ruff
{
    // countof(X)
    template<typename T, size_t N>
    constexpr size_t countof(const T (&)[N]) { return N; }
}

