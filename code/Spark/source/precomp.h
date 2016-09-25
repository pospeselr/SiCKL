#pragma once

// C++
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <csignal>
#include <vector>
#include <unordered_set>
#include <functional>

// OpenCL
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif