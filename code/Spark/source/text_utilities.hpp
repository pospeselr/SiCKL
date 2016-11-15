#pragma once

namespace Spark
{
    namespace Internal
    {
        template<typename... Args>
        int32_t doSnprintf(char* buffer, int32_t buffer_size, int32_t written, const Args&... args)
        {
            auto nullOrOffset = [](char* buffer, uint32_t offset) -> char*
            {
                return buffer + (buffer ? 1 : 0) * offset;
            };

            SPARK_ASSERT(written >= 0);
            auto len = snprintf(nullOrOffset(buffer, written),  buffer_size == 0 ? 0 : buffer_size - written, args...);
            SPARK_ASSERT(len >= 0);
            SPARK_ASSERT(buffer_size == 0 || (written + len < buffer_size));

            return written + len;
        }
    }
}