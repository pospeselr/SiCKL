#pragma once

namespace spark
{
    namespace lib
    {
        struct spark_context;
        struct spark_kernel;
        struct spark_buffer;
    }
}

typedef struct spark::lib::spark_context spark_context_t;
typedef struct spark::lib::spark_kernel spark_kernel_t;
typedef struct spark::lib::spark_buffer spark_buffer_t;