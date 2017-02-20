#pragma once

namespace spark
{
    namespace lib
    {
        struct spark_context;
        struct spark_kernel;
    }
}

typedef struct spark::lib::spark_context spark_context_t;
typedef struct spark::lib::spark_kernel spark_kernel_t;