#pragma once

typedef struct thistle_error thistle_error_t;
extern "C" void thistle_free_error(thistle_error_t*);
extern "C" const char* thistle_get_error_message(thistle_error_t*);

using thistle_throw_on_error =
    ruff::throw_on_error<
    thistle_error_t,
    decltype(&::thistle_free_error),
    &::thistle_free_error,
    decltype(&::thistle_get_error_message),
    &::thistle_get_error_message>;

#define THISTLE_THROW_ON_ERROR() thistle_throw_on_error()

extern "C" void thistle_begin_session(thistle_error_t**);
extern "C" void thistle_end_session(thistle_error_t**);

#if 0
typedef struct thistle_node thistle_node_t;
typedef struct thistle_node_dimensions
{
    size_t input_width;
    size_t input_height;
    size_t input_channels;
    size_t output_width;
    size_t output_height;
    size_t output_channels;
} thistle_node_dimensions_t;
#endif

typedef struct thistle_sample_batch thistle_sample_batch_t;
extern "C" size_t thistle_get_batch_size(thistle_sample_batch_t* batch, thistle_error_t** error);
extern "C" size_t thistle_get_sample_width(thistle_sample_batch_t* batch, thistle_error_t** error);
extern "C" size_t thistle_get_sample_height(thistle_sample_batch_t* batch, thistle_error_t** error);
extern "C" size_t thistle_get_sample_channels(thistle_sample_batch_t* batch, thistle_error_t** error);
extern "C" void thistle_get_data(thistle_sample_batch_t* batch, float* buffer, size_t bufferLength, thistle_error_t** error);
extern "C" thistle_sample_batch_t* thistle_create_sample_batch(size_t width, size_t height, size_t channels, size_t batchSize, float* data, thistle_error_t** error);


