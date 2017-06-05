#include "thistle.hpp"

#include "thistle_error.hpp"
#include "thistle_buffer.hpp"
#include "thistle_node.hpp"
#include "thistle_parameter_updater.hpp"
#include "thistle_sgd_parameter_updater.hpp"

using ruff::translate_exceptions;

thistle_sgd_parameter_updater::thistle_sgd_parameter_updater(size_t parameterCount)
: learning_rate(0.0f)
, momentum(0.0f)
, _prev_parameter_update_buffer(parameterCount)
, _update_parameters_kernel([=]()
{
    auto entry = MakeFunction([&](BufferView1D<Float> params, BufferView1D<Float> deltas, BufferView1D<Float> prevDeltas, Float learningRate, Float momentum)
    {
        Comment("SGD Parameter Update");
        Int idx = Index().X;

        Float dx = deltas[idx];
        Float v = prevDeltas[idx];
        Float x = params[idx];

        v = momentum * v + learningRate * dx;
        prevDeltas[idx] = v;
        params[idx] = x + v;
    });
    entry.SetEntryPoint();
})
{
    _update_parameters_kernel.set_work_dimensions(parameterCount);
}

void thistle_sgd_parameter_updater::update_parameters(
    thistle_node_t* node,
    const device_buffer1d<float>& parameterUpdateBuffer) const
{
    auto pParamBuffer = node->get_parameter_buffer();
    RUFF_THROW_IF_NULL(pParamBuffer);

    const auto paramCount = _prev_parameter_update_buffer.count();
    RUFF_THROW_IF_FALSE(paramCount == pParamBuffer->count());
    RUFF_THROW_IF_FALSE(paramCount == parameterUpdateBuffer.count());

    _update_parameters_kernel(*pParamBuffer, parameterUpdateBuffer, _prev_parameter_update_buffer, learning_rate, momentum);
}


RUFF_EXPORT thistle_parameter_updater_t* thistle_create_sgd_parameter_updater(
    size_t parameterCount,
    thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        RUFF_THROW_IF_FALSE(parameterCount > 0);

        return new thistle_sgd_parameter_updater(parameterCount);
    });
}

RUFF_EXPORT void thistle_set_sgd_parameter_updater_hyperparameters(
    thistle_parameter_updater_t* updater,
    float learningRate,
    float momentum,
    thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        auto sgdUpdater = dynamic_cast<thistle_sgd_parameter_updater*>(updater);
        RUFF_THROW_IF_NULL(sgdUpdater);

        RUFF_THROW_IF_FALSE(learningRate >= 0.0f);
        RUFF_THROW_IF_FALSE(momentum >= 0.0f);

        sgdUpdater->learning_rate = learningRate;;
        sgdUpdater->momentum = momentum;
    });
}