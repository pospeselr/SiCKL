#pragma once

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

extern "C" thistle_node_t* thistle_create_node_linear_transform(size_t inputs, size_t outputs);
extern "C" void thistle_get_node_dimensions(const thistle_node_t* node, thistle_node_dimensions_t* dimensions);


