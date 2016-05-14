#pragma once

// C++
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <csignal>
#include <vector>
#include <unordered_set>

// OpenCL
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif