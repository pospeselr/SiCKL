#pragma once

// linear transform node
struct thistle_linear_transform_node : public thistle_node
{
    thistle_linear_transform_node(
        size_t inputs,
        size_t outputs,
        const float* weights,
        size_t weight_count);

    void calc_output(
        const thistle_sample_batch_t* inputBatch,
        thistle_sample_batch_t* outputBatch) const override;

    size_t inputs() const {return _weights.width() - 1;}    // bias not included
    size_t outputs() const {return _weights.height();}

    device_buffer2d<float> _weights;
    mutable Kernel<Void(Buffer2D<Float>, Buffer2D<Float>, Buffer2D<Float>)> _kernel;
};