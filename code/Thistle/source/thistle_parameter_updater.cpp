#include "thistle.hpp"

#include "thistle_error.hpp"
#include "thistle_buffer.hpp"
#include "thistle_node.hpp"
#include "thistle_parameter_updater.hpp"

using ruff::translate_exceptions;

RUFF_EXPORT void thistle_update_node_parameters(
    const thistle_parameter_updater_t* parameterUpdater,
    thistle_node_t* node,
    const thistle_buffer_t* paramDeltas,
    thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(parameterUpdater);
        RUFF_THROW_IF_NULL(node);
        RUFF_THROW_IF_NULL(paramDeltas);

        parameterUpdater->update_parameters(node, paramDeltas->data);
    });
}

RUFF_EXPORT void thistle_free_parameter_updater(
    thistle_parameter_updater_t* updater,
    thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        delete updater;
    });
}