#include "thistle.hpp"

#include "thistle_error.hpp"
#include "thistle_buffer.hpp"
#include "thistle_node.hpp"
#include "thistle_linear_transform_node.hpp"

using ruff::translate_exceptions;

thistle_linear_transform_node::thistle_linear_transform_node(
    size_t inputs,
    size_t outputs,
    const float* weights,
    size_t weight_count)
: _weights(inputs + 1, outputs, weights)
, _calc_output_kernel([&]()
{
    auto entry = MakeFunction([&](Buffer2D<Float> weights, Buffer2D<Float> input_buffer, Buffer2D<Float> output_buffer)
    {
        Int2 idx = Index();
        Int batch_idx = idx.Y;
        Int feature_idx = idx.X;

        BufferView1D<Float> feature = weights[feature_idx];
        BufferView1D<Float> input = input_buffer[batch_idx];

        Float sum = 0.0f;
        For(Int j : Range<Int>(0, inputs))
        {
            Float x_j = input[j];
            Float w_ij = feature[j];
            sum = sum + (x_j * w_ij);
        }
        // finally add bias
        sum = sum + feature.Last();

        output_buffer[idx] = sum;
    });
    entry.SetEntryPoint();
})
{
    RUFF_THROW_IF_FALSE(weight_count == ((inputs + 1) * outputs));
}

size_t thistle_linear_transform_node::get_parameter_count() const
{
    return _weights.count();
}

void thistle_linear_transform_node::calc_output(
    const thistle_buffer_t* inputBatch,
    const thistle_buffer_t* constants,
    thistle_buffer_t* outputBatch) const
{
    // validate pointers
    RUFF_THROW_IF_NULL(inputBatch);
    RUFF_THROW_IF_FALSE(constants == nullptr);
    RUFF_THROW_IF_NULL(outputBatch);

    // validate input
    RUFF_THROW_IF_FALSE(inputBatch->batch_size == outputBatch->batch_size);
    RUFF_THROW_IF_FALSE(inputBatch->sample_size() == this->inputs());
    RUFF_THROW_IF_FALSE(outputBatch->sample_size() == this->outputs());
    RUFF_THROW_IF_FALSE(outputBatch->sample_height == 1);
    RUFF_THROW_IF_FALSE(outputBatch->sample_channels == 1);

    const auto batchSize = inputBatch->batch_size;

    _calc_output_kernel.set_work_dimensions(this->outputs(), batchSize);
    device_buffer2d<float> input_buffer(inputBatch->sample_size(), inputBatch->batch_size, inputBatch->data);
    device_buffer2d<float> output_buffer(outputBatch->sample_size(), outputBatch->batch_size, outputBatch->data);
    _calc_output_kernel(_weights, input_buffer, output_buffer);
}

void thistle_linear_transform_node::calc_parameter_deltas(
    const thistle_buffer_t* inputBatch,
    const thistle_buffer_t* outputBatchDelta,
    const thistle_buffer_t* constants,
    thistle_buffer_t* parameterDeltas) const
{

}

void thistle_linear_transform_node::calc_input_deltas(
    const thistle_buffer_t* inputBatch,
    const thistle_buffer_t* outputBatchDelta,
    const thistle_buffer_t* constants,
    thistle_buffer_t* inputDeltas) const
{

}

RUFF_EXPORT thistle_node_t* thistle_create_linear_transform_node(size_t inputs, size_t outputs, const float* weights, size_t weight_count, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        return new thistle_linear_transform_node(inputs, outputs, weights, weight_count);
    });
}