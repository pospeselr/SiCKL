#include "thistle.hpp"

#include "thistle_error.hpp"
#include "thistle_buffer.hpp"
#include "thistle_node.hpp"
#include "thistle_parameter_updater.hpp"

using ruff::translate_exceptions;

RUFF_EXPORT void thistle_update_node_parameters(
    const thistle_parameter_updater_t* parameterUpdater,
    thistle_buffer_t* paramBuffer,
    const thistle_buffer_t* paramDeltaBuffer,
    thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_NULL(parameterUpdater);
        RUFF_THROW_IF_NULL(paramBuffer);
        RUFF_THROW_IF_NULL(paramDeltaBuffer);

        parameterUpdater->update_parameters(paramBuffer->data, paramDeltaBuffer->data);
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