#include "thistle.hpp"

#include "thistle_error.hpp"
#include "thistle_buffer.hpp"
#include "thistle_node.hpp"
#include "thistle_label_node.hpp"

using ruff::translate_exceptions;

thistle_label_node::thistle_label_node(
    size_t labels,
    thistle_cost_function_t costFunction)
: labels(labels)
, cost_function(costFunction)
, _calc_error_kernel([&]()
{
    RUFF_THROW_IF_FALSE(costFunction == thistle_square_difference);
    auto entry = MakeFunction([&](Buffer2D<Float> calculated_buffer, Buffer2D<Float> expected_buffer, BufferView1D<Float> error_buffer)
    {
        Int2 idx = Index();
        Int batch_idx = idx.X;

        BufferView1D<Float> calculated = calculated_buffer[batch_idx];
        BufferView1D<Float> expected = expected_buffer[batch_idx];

        Float sum = 0.0f;
        For(Int j : Range<Int>(0, labels))
        {
            Float c_j = calculated[j];
            Float e_j = expected[j];

            Float diff = c_j - e_j;
            Float sq_diff = diff * diff;
            sum = sum + sq_diff;
        }
        Float mean_square = sum / Float(labels);
        error_buffer[batch_idx] = mean_square;
    });
    entry.SetEntryPoint();
}),
_calc_input_deltas_kernel([&]()
{
    RUFF_THROW_IF_FALSE(costFunction == thistle_square_difference);
    auto entry = MakeFunction([&](Buffer2D<Float> calculated_buffer, Buffer2D<Float> expected_buffer, Buffer2D<Float> input_deltas)
    {
        Int2 idx = Index();
        /*

        E = (x - l)^2
        dE/dx = 2 * (x - l)
        negative to minimize error

        */
        Float x = calculated_buffer[idx];
        Float l = expected_buffer[idx];

        input_deltas[idx] = -2.0f * (x - l);
    });
    entry.SetEntryPoint();
})
{

}

size_t thistle_label_node::get_parameter_count() const
{
    return 0;
}

thistle_buffer_t* thistle_label_node::get_parameter_buffer()
{
    return nullptr;
}

void thistle_label_node::calc_output(
    const thistle_buffer_t* inputBatch,
    const thistle_buffer_t* labelBatch,
    thistle_buffer_t* outputBatch) const
{
    // validate pointers
    RUFF_THROW_IF_NULL(inputBatch);
    RUFF_THROW_IF_NULL(labelBatch);
    RUFF_THROW_IF_NULL(outputBatch);

    // validate input
    RUFF_THROW_IF_FALSE(inputBatch->element_size() == this->labels);
    RUFF_THROW_IF_FALSE(labelBatch->element_size() == this->labels);
    // batch sizes
    RUFF_THROW_IF_FALSE(inputBatch->element_count == labelBatch->element_count);
    RUFF_THROW_IF_FALSE(inputBatch->element_count == outputBatch->element_count);
    const auto batchSize = inputBatch->element_count;
    // label dimensions
    RUFF_THROW_IF_FALSE(inputBatch->element_width == labelBatch->element_width);
    RUFF_THROW_IF_FALSE(inputBatch->element_height == labelBatch->element_height);
    RUFF_THROW_IF_FALSE(inputBatch->element_channels == labelBatch->element_channels);
    // output gets squished to 1 dimension
    RUFF_THROW_IF_FALSE(outputBatch->element_size() == 1);

    device_buffer2d<float> input_buffer(this->labels, batchSize, inputBatch->data);
    device_buffer2d<float> label_buffer(this->labels, batchSize, labelBatch->data);

    _calc_error_kernel.set_work_dimensions(this->labels);
    _calc_error_kernel(input_buffer, label_buffer, outputBatch->data);
}

void thistle_label_node::calc_parameter_deltas(
    const thistle_buffer_t* inputBatch,
    const thistle_buffer_t* outputBatchDelta,
    const thistle_buffer_t* constants,
    thistle_buffer_t* parameterDeltas) const
{
    // no-op
}

void thistle_label_node::calc_input_deltas(
    const thistle_buffer_t* inputBatch,
    const thistle_buffer_t* outputBatchDelta,
    const thistle_buffer_t* constants,
    thistle_buffer_t* inputBatchDeltas) const
{
    // validate pointers
    RUFF_THROW_IF_NULL(inputBatch);
    RUFF_THROW_IF_FALSE(outputBatchDelta == nullptr);
    RUFF_THROW_IF_FALSE(constants);
    RUFF_THROW_IF_NULL(inputBatchDeltas);

    // validate input
    RUFF_THROW_IF_FALSE(inputBatch->element_size() == constants->element_size());
    RUFF_THROW_IF_FALSE(inputBatch->element_size() == inputBatchDeltas->element_size());
    // batch sizes
    RUFF_THROW_IF_FALSE(inputBatch->element_count == constants->element_count);
    RUFF_THROW_IF_FALSE(inputBatch->element_count == inputBatchDeltas->element_count);

    const auto batchSize = inputBatch->element_count;
    // label dimensions
    RUFF_THROW_IF_FALSE(inputBatch->element_width == constants->element_width);
    RUFF_THROW_IF_FALSE(inputBatch->element_height == constants->element_height);
    RUFF_THROW_IF_FALSE(inputBatch->element_channels == constants->element_channels);
    RUFF_THROW_IF_FALSE(inputBatch->element_width == inputBatchDeltas->element_width);
    RUFF_THROW_IF_FALSE(inputBatch->element_height == inputBatchDeltas->element_height);
    RUFF_THROW_IF_FALSE(inputBatch->element_channels == inputBatchDeltas->element_channels);

    device_buffer2d<float> input_buffer(this->labels, batchSize, inputBatch->data);
    device_buffer2d<float> label_buffer(this->labels, batchSize, constants->data);
    device_buffer2d<float> input_delta_buffer(this->labels, batchSize, inputBatchDeltas->data);

    _calc_input_deltas_kernel.set_work_dimensions(this->labels, batchSize);
    _calc_input_deltas_kernel(input_buffer, label_buffer, input_delta_buffer);
}

RUFF_EXPORT thistle_node_t* thistle_create_label_node(
    size_t labelCount,
    thistle_cost_function_t costFunction,
    thistle_error_t** error)
{
    return translate_exceptions(error, [&]()
    {
        return new thistle_label_node(labelCount, costFunction);
    });
}
