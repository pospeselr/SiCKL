#pragma once

// std
#include <csignal>
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <functional>
#include <vector>
#include <memory>
#include <stdexcept>
#include <utility>
#include <unordered_set>

// spark
#include "spark/platform.h"
#include "spark/utilities.h"
#include "spark/enums.h"

// opencl
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>

