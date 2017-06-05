#pragma once

struct thistle_label_node : public thistle_node
{
    thistle_label_node(
        size_t labels,
        thistle_cost_function_t costFunction);

    ~thistle_label_node() = default;

    // thistle_node interface
    size_t get_parameter_count() const override;
    device_buffer1d<float>* get_parameter_buffer() override;
    const device_buffer1d<float>* get_parameter_buffer() const override;
    void calc_output(
        const thistle_buffer_t* inputBatch,
        const thistle_buffer_t* constants,
        thistle_buffer_t* outputBatch) const override;
    void calc_parameter_deltas(
        const thistle_buffer_t* inputBatch,
        const thistle_buffer_t* outputBatchDeltas,
        const thistle_buffer_t* constants,
        thistle_buffer_t* parameterDeltas) const override;
    void calc_input_deltas(
        const thistle_buffer_t* inputBatch,
        const thistle_buffer_t* outputBatchDeltas,
        const thistle_buffer_t* constants,
        thistle_buffer_t* inputBatchDeltas) const override;

    const size_t labels;
    const thistle_cost_function_t cost_function;
private:
    mutable Kernel<Void(Buffer2D<Float>, Buffer2D<Float>, BufferView1D<Float>)> _calc_error_kernel;
    mutable Kernel<Void(Buffer2D<Float>, Buffer2D<Float>, Buffer2D<Float>)> _calc_input_deltas_kernel;
};