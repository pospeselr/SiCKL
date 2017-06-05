#pragma once

// sgd update with learning rate and momentum
struct thistle_sgd_parameter_updater : public thistle_parameter_updater
{
    thistle_sgd_parameter_updater(size_t parameterCount);
    ~thistle_sgd_parameter_updater() = default;

    // interface methods
    void update_parameters(
        thistle_node_t* node,
        const device_buffer1d<float>& parameterUpdateBuffer) const override;

    float learning_rate;
    float momentum;

private:

    device_buffer1d<float> _prev_parameter_update_buffer;
    Kernel<Void(BufferView1D<Float>,BufferView1D<Float>,BufferView1D<Float>, Float, Float)> _update_parameters_kernel;
};
