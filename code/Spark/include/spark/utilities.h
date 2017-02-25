#pragma once

#define TRACE() printf("%s(%u) : %s\n", __FILE__, (uint32_t)__LINE__, __PRETTY_FUNCTION__)

// countof(X)
template<typename T, size_t N>
constexpr size_t countof(const T (&)[N]) { return N; }
