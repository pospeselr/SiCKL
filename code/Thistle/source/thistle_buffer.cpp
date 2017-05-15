#include "thistle.hpp"

#include "thistle_error.hpp"
#include "thistle_buffer.hpp"

using ruff::translate_exceptions;

RUFF_EXPORT size_t thistle_get_buffer_batch_size(thistle_buffer_t* buffer, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(buffer);
        return buffer->batch_size;
    });
}

RUFF_EXPORT size_t thistle_get_buffer_sample_width(thistle_buffer_t* buffer, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(buffer);
        return buffer->sample_width;
    });
}

RUFF_EXPORT size_t thistle_get_buffer_sample_height(thistle_buffer_t* buffer, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(buffer);
        return buffer->sample_height;
    });
}

RUFF_EXPORT size_t thistle_get_buffer_sample_channels(thistle_buffer_t* buffer, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(buffer);
        return buffer->sample_channels;
    });
}

RUFF_EXPORT void thistle_get_buffer_data(thistle_buffer_t* buffer, size_t bufferLength, float* dest, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(buffer);
        RUFF_THROW_IF_NULL(dest);
        RUFF_THROW_IF_FALSE(bufferLength >= buffer->data.count());

        buffer->data.read(dest);
    });
}

RUFF_EXPORT void thistle_set_buffer_data(thistle_buffer* buffer, size_t bufferLength, const float* data, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(buffer);
        RUFF_THROW_IF_FALSE(bufferLength >= buffer->data.count());

        buffer->data.write(data);
    });
}

RUFF_EXPORT void thistle_zero_buffer(thistle_buffer_t* buffer, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(buffer);

        buffer->data.zero();
    });
}

RUFF_EXPORT thistle_buffer_t* thistle_create_sample_buffer(size_t width, size_t height, size_t channels, size_t batchSize, const float* data, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_FALSE(width > 0);
        RUFF_THROW_IF_FALSE(height > 0);
        RUFF_THROW_IF_FALSE(channels > 0);
        RUFF_THROW_IF_FALSE(batchSize > 0);

        return new thistle_buffer(width, height, channels, batchSize, data);
    });
}

RUFF_EXPORT thistle_buffer_t* thistle_create_flat_buffer(size_t count, const float* data, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_FALSE(count > 0);

        return new thistle_buffer(count, 1, 1, 1, data);
    });
}

RUFF_EXPORT void thistle_free_buffer(thistle_buffer_t* buffer, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        delete buffer;
    });
}