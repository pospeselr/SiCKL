#pragma once

// linear transform node
struct thistle_linear_transform_node : public thistle_node
{
    thistle_linear_transform_node(
        size_t inputs,
        size_t outputs,
        const float* weights,
        size_t weight_count);

    ~thistle_linear_transform_node() = default;

    // thistle_node interface
    size_t get_parameter_count() const override;
    thistle_buffer_t* get_parameter_buffer() override;
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

    // helper methods
    size_t inputs() const {return _weights.width() - 1;}    // bias not included
    size_t outputs() const {return _weights.height();}

    // data
private:
    device_buffer2d<float> _weights;
    mutable Kernel<Void(Buffer2D<Float>, Buffer2D<Float>, Buffer2D<Float>)> _calc_output_kernel;
    mutable Kernel<Void(Buffer2D<Float>, Buffer2D<Float>, Buffer2D<Float>)> _calc_parameter_deltas_kernel;
    mutable Kernel<Void(Buffer2D<Float>, Buffer2D<Float>, Buffer2D<Float>)> _calc_input_deltas_kernel;
};