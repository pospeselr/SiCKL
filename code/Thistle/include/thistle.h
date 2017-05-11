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

// sample batch type and functions
typedef struct thistle_sample_batch thistle_sample_batch_t;
extern "C" size_t thistle_get_batch_size(thistle_sample_batch_t* batch, thistle_error_t** error);
extern "C" size_t thistle_get_sample_width(thistle_sample_batch_t* batch, thistle_error_t** error);
extern "C" size_t thistle_get_sample_height(thistle_sample_batch_t* batch, thistle_error_t** error);
extern "C" size_t thistle_get_sample_channels(thistle_sample_batch_t* batch, thistle_error_t** error);
extern "C" void thistle_get_sample_data(thistle_sample_batch_t* batch, size_t bufferLength, float* buffer, thistle_error_t** error);
extern "C" void thistle_set_sample_data(thistle_sample_batch_t* batch, size_t bufferLength, const float* buffer, thistle_error_t** error);
extern "C" thistle_sample_batch_t* thistle_create_sample_batch(size_t width, size_t height, size_t channels, size_t batchSize, float* data, thistle_error_t** error);
extern "C" void thistle_free_sample_batch(thistle_sample_batch_t* batch, thistle_error_t**);

// thistle node type and functions
typedef struct thistle_node thistle_node_t;
extern "C" void thistle_calc_output(thistle_node_t* node, const thistle_sample_batch_t* inputBatch, thistle_sample_batch_t* outputBatch, thistle_error_t** error);

// construction functions for various node types
extern "C" thistle_node_t* thistle_create_linear_transform_node(size_t inputs, size_t outputs, const float* weights, size_t weight_count, thistle_error_t** error);