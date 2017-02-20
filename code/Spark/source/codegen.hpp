#pragma once

namespace spark
{
    namespace lib
    {
        int32_t generateOpenCLSource(spark_node_t* node, char* out_buffer, int32_t buffer_size);
    }
}