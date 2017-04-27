#pragma once

struct thistle_sample_batch
{
    thistle_sample_batch(
        size_t sampleWidth,
        size_t sampleHeight,
        size_t sampleChannels,
        size_t batchSize,
        float* dataBuffer)
    : _sample_width(sampleWidth)
    , _sample_height(sampleHeight)
    , _sample_channels(sampleChannels)
    , _batch_size(batchSize)
    , _data(sampleWidth * sampleHeight * sampleChannels * batchSize, dataBuffer)
    { }

    const size_t _sample_width;
    const size_t _sample_height;
    const size_t _sample_channels;
    const size_t _batch_size;

    spark::device_buffer1d<float> _data;
};
typedef thistle_sample_batch thistle_sample_batch_t;