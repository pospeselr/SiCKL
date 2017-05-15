#include "thistle.hpp"

#include "thistle_error.hpp"
#include "thistle_buffer.hpp"
#include "thistle_buffer.hpp"
#include "thistle_node.hpp"

using ruff::translate_exceptions;

RUFF_EXPORT size_t thistle_get_node_parameters_count(thistle_node_t* node, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(node);

        return node->get_parameter_count();
    });
}

RUFF_EXPORT void thistle_calc_node_output(thistle_node_t* node, const thistle_buffer_t* inputBatch, thistle_buffer_t* outputBatch, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(node);
        RUFF_THROW_IF_NULL(inputBatch);
        RUFF_THROW_IF_NULL(outputBatch);

        node->calc_output(inputBatch, outputBatch);
    });
}

//RUFF_EXPORT void thistle_calc_node_parameter_deltas(thistle_node_t* node, )