#include "thistle.hpp"

#include "thistle_error.hpp"
#include "thistle_sample_batch.hpp"
#include "thistle_node.hpp"
#include "thistle_linear_transform_node.hpp"

using ruff::translate_exceptions;

RUFF_EXPORT void thistle_calc_output(thistle_node_t* node, const thistle_sample_batch_t* inputBatch, thistle_sample_batch_t* outputBatch, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(node);
        RUFF_THROW_IF_NULL(inputBatch);
        RUFF_THROW_IF_NULL(outputBatch);

        node->calc_output(inputBatch, outputBatch);
    });
}