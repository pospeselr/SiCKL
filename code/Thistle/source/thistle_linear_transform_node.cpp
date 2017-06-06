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
        Comment("Linear Transform Outputs");
        Int2 idx = Index();
        Int batch_idx = idx.Y;
        Int j = idx.X;

        BufferView1D<Float> w_j = weights[j];
        BufferView1D<Float> x = input_buffer[batch_idx];

        Float sum = 0.0f;
        For(Int i : Range<Int>(0, inputs))
        {
            Float x_i = x[i];
            Float w_ji = w_j[i];
            sum = sum + (w_ji * x_i);
        }
        // finally add bias
        sum = sum + w_j.Last();

        output_buffer[idx] = sum;
    });
    entry.SetEntryPoint();
})
, _calc_parameter_deltas_kernel([&]()
{
    auto entry = MakeFunction([&](Buffer2D<Float> input_buffer, Buffer2D<Float> output_delta_buffer, Buffer2D<Float> weight_delta_buffer)
    {
        Comment("Linaer Transform Parameter Deltas");
        Int2 idx = Index();
        Int i = idx.X;
        Int j = idx.Y;
        // caller verifies batch size is the same
        Int batches = input_buffer.Height;

        // averaged over batch size
        Float sum = 0.0f;

        // bias
        If(i == inputs)
        {
            Comment("Bias");
            For(Int b : Range<Int>(0, batches))
            {
                BufferView1D<Float> dy = output_delta_buffer[b];

                Float dy_j = dy[j];

                sum = sum + dy_j;
            }
        }

        // regular weight
        Else
        {
            Comment("Regular Weight");
            For(Int b : Range<Int>(0, batches))
            {
                BufferView1D<Float> x = input_buffer[b];
                BufferView1D<Float> dy = output_delta_buffer[b];

                Float x_i = x[i];
                Float dy_j = dy[j];

                sum = sum + (x_i * dy_j);
            }
        }

        // average over batch
        sum = sum / batches.As<Float>();

        weight_delta_buffer[idx] = sum;
    });
    entry.SetEntryPoint();
})
, _calc_input_deltas_kernel([&]()
{
    auto entry = MakeFunction([&](Buffer2D<Float> weights, Buffer2D<Float> output_delta_buffer, Buffer2D<Float> input_delta_buffer)
    {
        Comment("Linear Transform Input Deltas");
        Int2 idx = Index();
        Int batch_idx = idx.Y;
        Int i = idx.X;

        BufferView1D<Float> dy = output_delta_buffer[batch_idx];
        auto& w = weights;

        Float sum = 0.0f;
        For(Int j : Range<Int>(0, outputs))
        {
            Float dy_j = dy[j];
            Float w_ji = w[j][i];

            sum = sum + (w_ji * dy_j);
        }

        input_delta_buffer[idx] = sum;
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

thistle_buffer_t* thistle_linear_transform_node::get_parameter_buffer()
{
    return new thistle_buffer(_weights.width(), _weights.height(), 1, 1, _weights);
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

    device_buffer2d<float> input_buffer(inputBatch->sample_size(), batchSize, inputBatch->data);
    device_buffer2d<float> output_buffer(outputBatch->sample_size(), batchSize, outputBatch->data);

    _calc_output_kernel.set_work_dimensions(this->outputs(), batchSize);
    _calc_output_kernel(_weights, input_buffer, output_buffer);
}

void thistle_linear_transform_node::calc_parameter_deltas(
    const thistle_buffer_t* inputBatch,
    const thistle_buffer_t* outputBatchDelta,
    const thistle_buffer_t* constants,
    thistle_buffer_t* parameterDeltas) const
{
    // validate pointers
    RUFF_THROW_IF_NULL(inputBatch);
    RUFF_THROW_IF_NULL(outputBatchDelta);
    RUFF_THROW_IF_NOT_NULL(constants);
    RUFF_THROW_IF_NULL(parameterDeltas);

    // validate th einput
    RUFF_THROW_IF_FALSE(inputBatch->batch_size == outputBatchDelta->batch_size);
    RUFF_THROW_IF_FALSE(inputBatch->sample_size() == this->inputs());
    RUFF_THROW_IF_FALSE(outputBatchDelta->sample_size() == this->outputs());
    RUFF_THROW_IF_FALSE(outputBatchDelta->sample_height == 1);
    RUFF_THROW_IF_FALSE(outputBatchDelta->sample_channels == 1);
    RUFF_THROW_IF_FALSE(parameterDeltas->data.count() == _weights.count());
    RUFF_THROW_IF_FALSE(parameterDeltas->sample_width == parameterDeltas->sample_size());

    const auto batchSize = inputBatch->batch_size;

    device_buffer2d<float> input_buffer(inputBatch->sample_size(), batchSize, inputBatch->data);
    device_buffer2d<float> output_delta_buffer(outputBatchDelta->sample_size(), batchSize, outputBatchDelta->data);
    device_buffer2d<float> weight_delta_buffer(_weights.width(), _weights.height(), parameterDeltas->data);

    _calc_parameter_deltas_kernel.set_work_dimensions(weight_delta_buffer.width(), weight_delta_buffer.height());
    _calc_parameter_deltas_kernel(input_buffer, output_delta_buffer, weight_delta_buffer);
}

void thistle_linear_transform_node::calc_input_deltas(
    const thistle_buffer_t* inputBatch,
    const thistle_buffer_t* outputBatchDelta,
    const thistle_buffer_t* constants,
    thistle_buffer_t* inputBatchDelta) const
{
    // validate pointers
    RUFF_THROW_IF_NOT_NULL(inputBatch);
    RUFF_THROW_IF_NULL(outputBatchDelta);
    RUFF_THROW_IF_NOT_NULL(constants);
    RUFF_THROW_IF_NULL(inputBatchDelta);

    // validate input
    RUFF_THROW_IF_FALSE(inputBatchDelta->batch_size == outputBatchDelta->batch_size);
    RUFF_THROW_IF_FALSE(inputBatchDelta->sample_size() == this->inputs());
    RUFF_THROW_IF_FALSE(outputBatchDelta->sample_size() == this->outputs());
    RUFF_THROW_IF_FALSE(outputBatchDelta->sample_height == 1);
    RUFF_THROW_IF_FALSE(outputBatchDelta->sample_channels == 1);

    const auto batchSize = inputBatchDelta->batch_size;

    device_buffer2d<float> output_delta_buffer(outputBatchDelta->sample_size(), batchSize, outputBatchDelta->data);
    device_buffer2d<float> input_delta_buffer(inputBatchDelta->sample_size(), batchSize, inputBatchDelta->data);

    _calc_input_deltas_kernel.set_work_dimensions(this->inputs(), batchSize);
    _calc_input_deltas_kernel(_weights, output_delta_buffer, input_delta_buffer);
}

RUFF_EXPORT thistle_node_t* thistle_create_linear_transform_node(size_t inputs, size_t outputs, const float* weights, size_t weight_count, thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        return new thistle_linear_transform_node(inputs, outputs, weights, weight_count);
    });
}