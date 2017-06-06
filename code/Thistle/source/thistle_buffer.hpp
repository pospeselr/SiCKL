#pragma once

struct thistle_buffer
{
    thistle_buffer(
        size_t elementWidth,
        size_t elementHeight,
        size_t elementChannels,
        size_t elementCount,
        const float* dataBuffer)
    : element_width(elementWidth)
    , element_height(elementHeight)
    , element_channels(elementChannels)
    , element_count(elementCount)
    , data(elementWidth * elementHeight * elementChannels * elementCount, dataBuffer)
    { }

    thistle_buffer(
        size_t elementWidth,
        size_t elementHeight,
        size_t elementChannels,
        size_t elementCount,
        spark::device_buffer1d<float>& data)
    : element_width(elementWidth)
    , element_height(elementHeight)
    , element_channels(elementChannels)
    , element_count(elementCount)
    , data(data)
    { }

    const size_t element_width;
    const size_t element_height;
    const size_t element_channels;
    const size_t element_count;

    size_t element_size() const
    {
        return element_width * element_height * element_channels;
    }

    spark::device_buffer1d<float> data;
};
typedef thistle_buffer thistle_buffer_t;