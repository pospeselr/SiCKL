#include "thistle.hpp"

#include "thistle_error.hpp"
#include "thistle_buffer.hpp"
#include "thistle_buffer.hpp"
#include "thistle_node.hpp"

using ruff::translate_exceptions;

RUFF_EXPORT size_t thistle_get_node_parameters_count(
    const thistle_node_t* node,
    thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(node);

        return node->get_parameter_count();
    });
}

RUFF_EXPORT void thistle_calc_node_output(
    const thistle_node_t* node,
    const thistle_buffer_t* inputBatch,
    const thistle_buffer_t* constants,
    thistle_buffer_t* outputBatch,
    thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(node);

        node->calc_output(inputBatch, constants, outputBatch);
    });
}

RUFF_EXPORT void thistle_calc_node_parameter_deltas(
    const thistle_node_t* node,
    const thistle_buffer_t* inputs,
    const thistle_buffer_t* outputDeltas,
    const thistle_buffer_t* constants,
    thistle_buffer_t* paramDeltas,
    thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(node);

        node->calc_parameter_deltas(inputs, outputDeltas, constants, paramDeltas);
    });
}

RUFF_EXPORT void thistle_calc_node_input_deltas(
    const thistle_node_t* node,
    const thistle_buffer_t* inputs,
    const thistle_buffer_t* outputDeltas,
    const thistle_buffer_t* constants,
    thistle_buffer_t* inptuDeltas,
    thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(node);

        node->calc_parameter_deltas(inputs, outputDeltas, constants, inptuDeltas);
    });
}

