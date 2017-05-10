#pragma once

// c++
#include <cstddef>
#include <stdexcept>
#include <memory>

// ruff
#include <ruff.h>

// spark
#define SPARK_NEVER_INLINE RUFF_NEVER_INLINE
#define SPARK_FORCE_INLINE RUFF_FORCE_INLINE
#define SPARK_DEBUGBREAK RUFF_DEBUGBRAEK
#define SPARK_ASSERT(X) RUFF_ASSERT(X)
#define SPARK_VERIFY(X) RUFF_VERIFY(X)
#include <spark.h>
using namespace spark;

