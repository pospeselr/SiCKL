#pragma once

// linear transform node
struct thistle_linear_transform_node : public thistle_node
{
    thistle_linear_transform_node(
        size_t inputs,
        size_t outputs,
        const float* weights,
        size_t weight_count);

    // thistle_node interface
    size_t get_parameter_count() const override;
    void calc_output(
        const thistle_buffer_t* inputBatch,
        thistle_buffer_t* outputBatch) const override;
    void calc_parameter_deltas(
        const thistle_buffer_t* inputBatch,
        const thistle_buffer_t* outputBatchDeltas,
        thistle_buffer_t* parameterDeltas) const override;

    // helper methods
    size_t inputs() const {return _weights.width() - 1;}    // bias not included
    size_t outputs() const {return _weights.height();}

    // data
    device_buffer2d<float> _weights;
    mutable Kernel<Void(Buffer2D<Float>, Buffer2D<Float>, Buffer2D<Float>)> _kernel;
};