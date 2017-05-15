#pragma once

struct thistle_buffer
{
    thistle_buffer(
        size_t sampleWidth,
        size_t sampleHeight,
        size_t sampleChannels,
        size_t batchSize,
        const float* dataBuffer)
    : sample_width(sampleWidth)
    , sample_height(sampleHeight)
    , sample_channels(sampleChannels)
    , batch_size(batchSize)
    , data(sampleWidth * sampleHeight * sampleChannels * batchSize, dataBuffer)
    { }

    const size_t sample_width;
    const size_t sample_height;
    const size_t sample_channels;
    const size_t batch_size;

    size_t sample_size() const
    {
        return sample_width * sample_height * sample_channels;
    }

    spark::device_buffer1d<float> data;
};
typedef thistle_buffer thistle_buffer_t;