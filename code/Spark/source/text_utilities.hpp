#pragma once

namespace spark
{
    namespace lib
    {
        template<typename T>
        struct codegen_context : public T
        {
            char* buffer = nullptr;
            int32_t capacity = 0;
            int32_t written = 0;
        };

        template<int32_t N, typename... Args>
        int32_t doSnprintf(char* buffer, int32_t buffer_size, int32_t written, const char (&format)[N], const Args... args)
        {
            auto nullOrOffset = [](char* buffer, uint32_t offset) -> char*
            {
                return buffer + (buffer ? 1 : 0) * offset;
            };

            SPARK_ASSERT(written >= 0);
            auto len = snprintf(nullOrOffset(buffer, written),  buffer_size == 0 ? 0 : buffer_size - written, format, args...);

            SPARK_ASSERT(len >= 0);
            SPARK_ASSERT(buffer_size == 0 || (written + len < buffer_size));

            return (written + len);
        }

        template<int32_t N>
        int32_t doSnprintf(char* buffer, int32_t buffer_size, int32_t written, const char (&literal)[N])
        {
            // N includes terminating null, which we don't count here
            const auto len = N - 1;
            if(buffer == nullptr)
            {
                SPARK_ASSERT(buffer_size == 0);
            }
            else
            {
                SPARK_ASSERT(written + len < buffer_size);
                memcpy(buffer + written, (void*)literal, (N));
            }
            return (written + len);
        }

        template<typename Context, int32_t N, typename... Args>
        void doSnprintf(codegen_context<Context>& ctx, const char (&format)[N], const Args... args)
        {
            ctx.written = doSnprintf(
                ctx.buffer,
                ctx.capacity,
                ctx.written,
                format,
                args...);
        }

        std::string string_format(const char* format, ...);
    }
}