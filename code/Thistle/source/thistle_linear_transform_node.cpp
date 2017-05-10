#include "thistle.hpp"

#include "thistle_error.hpp"
#include "thistle_sample_batch.hpp"
#include "thistle_node.hpp"
#include "thistle_linear_transform_node.hpp"

using ruff::translate_exceptions;

#include <iostream>
#define LOG_VAL(X) std::cout << #X " : " << (X) << std::endl

thistle_linear_transform_node::thistle_linear_transform_node(
    size_t inputs,
    size_t outputs,
    const float* weights,
    size_t weight_count)
: _weights(inputs + 1, outputs, weights)
, _kernel([&]()
{
    auto entry = MakeFunction([&](Buffer2D<Float> weights, Buffer2D<Float> input_buffer, Buffer2D<Float> output_buffer)
    {
        Int2 idx = Index();
        Int batch_idx = idx.Y;
        Int feature_idx = idx.X;

        BufferView1D<Float> feature = weights[feature_idx];
        BufferView1D<Float> input = input_buffer[batch_idx];

        // init to bias
        Float sum = feature.Last();
        For(Int j : Range<Int>(0, inputs))
        {
            Float x_j = input[j];
            Float w_ij = feature[j];
            sum = sum + (x_j * w_ij);
        }

        output_buffer[idx] = sum;
    });
    entry.SetEntryPoint();
})
{
    RUFF_THROW_IF_FALSE(weight_count == ((inputs + 1) * outputs));
}

void thistle_linear_transform_node::calc_output(const thistle_sample_batch_t* inputBatch, thistle_sample_batch_t* outputBatch) const
{
    RUFF_THROW_IF_FALSE(inputBatch->batch_size == outputBatch->batch_size);
    RUFF_THROW_IF_FALSE(inputBatch->sample_size() == this->inputs());
    RUFF_THROW_IF_FALSE(outputBatch->sample_size() == this->outputs());
    RUFF_THROW_IF_FALSE(outputBatch->sample_height == 1);
    RUFF_THROW_IF_FALSE(outputBatch->sample_channels == 1);

    const auto batchSize = inputBatch->batch_size;

    _kernel.set_work_dimensions(this->outputs(), batchSize);
    device_buffer2d<float> input_buffer(inputBatch->sample_size(), inputBatch->batch_size, inputBatch->data);
    device_buffer2d<float> output_buffer(outputBatch->sample_size(), outputBatch->batch_size, outputBatch->data);
    _kernel(_weights, input_buffer, output_buffer);
}

RUFF_EXPORT thistle_node_t* thistle_create_linear_transform_node(size_t inputs, size_t outputs, const float* weights, size_t weight_count, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        return new thistle_linear_transform_node(inputs, outputs, weights, weight_count);
    });
}