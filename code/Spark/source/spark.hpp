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
#include "spark/common.h"
#include "spark/error.h"
#include "spark/enums.h"
#include "spark/node.h"
#include "spark/codegen.h"

// opencl
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>