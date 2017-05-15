 #pragma once

typedef struct thistle_error thistle_error_t;
extern "C" void thistle_free_error(thistle_error_t*);
extern "C" const char* thistle_get_error_message(thistle_error_t*);

extern "C" void thistle_begin_session(thistle_error_t**);
extern "C" void thistle_end_session(thistle_error_t**);

// thistle buffer type and functions
typedef struct thistle_buffer thistle_buffer_t;
extern "C" size_t thistle_get_buffer_batch_size(thistle_buffer_t* buffer, thistle_error_t** error);
extern "C" size_t thistle_get_buffer_sample_width(thistle_buffer_t* buffer, thistle_error_t** error);
extern "C" size_t thistle_get_buffer_sample_height(thistle_buffer_t* buffer, thistle_error_t** error);
extern "C" size_t thistle_get_buffer_sample_channels(thistle_buffer_t* buffer, thistle_error_t** error);
extern "C" void thistle_get_buffer_data(thistle_buffer_t* buffer, size_t bufferLength, float* dest, thistle_error_t** error);
extern "C" void thistle_set_buffer_data(thistle_buffer_t* buffer, size_t bufferLength, const float* data, thistle_error_t** error);
extern "C" void thistle_zero_buffer(thistle_buffer_t* buffer, thistle_error_t** error);
extern "C" thistle_buffer_t* thistle_create_sample_buffer(size_t width, size_t height, size_t channels, size_t batchSize, const float* data, thistle_error_t** error);
extern "C" thistle_buffer_t* thistle_create_flat_buffer(size_t count, const float* data, thistle_error_t** error);
extern "C" void thistle_free_buffer(thistle_buffer_t* batch, thistle_error_t** error);

// node type and functions
typedef struct thistle_node thistle_node_t;
extern "C" size_t thistle_get_node_parameters_count(const thistle_node_t* node, thistle_error_t** error);
extern "C" void thistle_calc_node_output(const thistle_node_t* node, const thistle_buffer_t* inputBatch, const thistle_buffer_t* constants, thistle_buffer_t* outputBatch, thistle_error_t** error);
extern "C" void thistle_calc_node_parameter_deltas(const thistle_node_t* node, const thistle_buffer_t* inputs, const thistle_buffer_t* outputDeltas, const thistle_buffer_t* constants,  thistle_buffer_t* paramDeltas, thistle_error_t** error);
extern "C" void thistle_calc_node_input_deltas(const thistle_node_t* node, const thistle_buffer_t* inputs, const thistle_buffer_t* outputDeltas, const thistle_buffer_t* constants, thistle_buffer_t* inptuDeltas, thistle_error_t** error);

// construction functions for various node types
extern "C" thistle_node_t* thistle_create_linear_transform_node(size_t inputs, size_t outputs, const float* weights, size_t weight_count, thistle_error_t** error);
typedef enum thistle_cost_function
{
    square_difference = 0,
    cross_entropy     = 1,
} thistle_cost_function_t;
extern "C" thistle_node_t* thistle_create_label_node(size_t labelCount, thistle_cost_function_t costFunction, thistle_error_t** error);




// C++ helpers
#ifdef __cplusplus
using THISTLE_THROW_ON_ERROR =
    ruff::throw_on_error<
    thistle_error_t,
    decltype(&::thistle_free_error),
    &::thistle_free_error,
    decltype(&::thistle_get_error_message),
    &::thistle_get_error_message>;
#endif