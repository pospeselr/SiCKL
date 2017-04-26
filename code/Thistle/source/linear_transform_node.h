#pragma once

namespace thistle
{
    class linear_transform_node : public computation_node
    {
    public:
        linear_transform_node(size_t inputs, size_t outputs);
        // calculates outputs given an input vector and internal parameter buffer
        void calc_outputs(
            const spark::device_buffer1d<float>& inputs) const override;
#if 0
        // calculate parameter deltas based on provided inputs, output deltas, and internal parameter buffer
        void calc_parameter_deltas(
            const spark::device_buffer1d<float>& inputs,
            const spark::device_buffer1d<float>& output_deltas,
            spark::device_buffer1d<float>& param_deltas) const override;
        // calculate input deltas based on provided inputs, output deltas, and internal parameter buffer
        void calc_input_deltas(
            const spark::device_buffer1d<float>& inputs,
            const spark::device_buffer1d<float>& output_deltas,
            spark::device_buffer1d<float>& input_deltas) const override;

        size_t input_width() const override;
        size_t input_height() const override {return 1;}
        size_t input_channels() const override {return 1;}

        size_t output_width() const override;
        size_t output_height() const override {return 1;}
        size_t output_channels() const override {return 1;}
#endif
    };
}