#include "thistle.hpp"

#include "thistle_error.hpp"
#include "thistle_sample_batch.hpp"

using ruff::translate_exceptions;

RUFF_EXPORT size_t thistle_get_batch_size(thistle_sample_batch_t* batch, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(batch);
        return batch->batch_size;
    });
}

RUFF_EXPORT size_t thistle_get_sample_width(thistle_sample_batch_t* batch, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(batch);
        return batch->sample_width;
    });
}

RUFF_EXPORT size_t thistle_get_sample_height(thistle_sample_batch_t* batch, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(batch);
        return batch->sample_height;
    });
}

RUFF_EXPORT size_t thistle_get_sample_channels(thistle_sample_batch_t* batch, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(batch);
        return batch->sample_channels;
    });
}

RUFF_EXPORT void thistle_get_sample_data(thistle_sample_batch_t* batch, float* buffer, size_t bufferLength, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(batch);
        RUFF_THROW_IF_NULL(buffer);
        RUFF_THROW_IF_FALSE(bufferLength >= batch->data.count());

        batch->data.read(buffer);
    });
}

RUFF_EXPORT void thistle_set_sample_data(thistle_sample_batch* batch, const float* buffer, size_t bufferLength, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(batch);
        RUFF_THROW_IF_NULL(buffer);
        RUFF_THROW_IF_FALSE(bufferLength >= batch->data.count());

        batch->data.write(buffer);
    });
}

RUFF_EXPORT thistle_sample_batch_t* thistle_create_sample_batch(size_t width, size_t height, size_t channels, size_t batchSize, float* data, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_FALSE(width > 0);
        RUFF_THROW_IF_FALSE(height > 0);
        RUFF_THROW_IF_FALSE(channels > 0);
        RUFF_THROW_IF_FALSE(batchSize > 0);

        return new thistle_sample_batch(width, height, channels, batchSize, data);
    });
}

RUFF_EXPORT void thistle_free_sample_batch(thistle_sample_batch_t* batch, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        delete batch;
    });
}