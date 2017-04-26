#include "thistle.hpp"

struct thistle_node
{
    // calculates outputs given an input vector and internal parameter buffer
    virtual void calc_outputs(
        const spark::device_buffer1d<float>& inputs) const = 0;
#if 0
    // calculate parameter deltas based on provided inputs, output deltas, and internal parameter buffer
    virtual void calc_parameter_deltas(
        const spark::device_buffer1d<float>& inputs,
        const spark::device_buffer1d<float>& output_deltas,
        spark::device_buffer1d<float>& param_deltas) const = 0;
    // calculate input deltas based on provided inputs, output deltas, and internal parameter buffer
    virtual void calc_input_deltas(
        const spark::device_buffer1d<float>& inputs,
        const spark::device_buffer1d<float>& output_deltas,
        spark::device_buffer1d<float>& input_deltas) const = 0;

    virtual size_t input_width() const = 0;
    virtual size_t input_height() const = 0;
    virtual size_t input_channels() const = 0;

    virtual size_t output_width() const = 0;
    virtual size_t output_height() const = 0;
    virtual size_t output_channels() const = 0;

    spark::device_buffer1d<float>& get_parameters() { return _params; }
protected:
    spark::device_buffer1d<float> _params;
#endif
};
typedef thistle_node thistle_node_t;

RUFF_EXPORT thistle_node_t* thistle_create_node_linear_transform(size_t inputs, size_t outputs)
{
    return nullptr;
}

RUFF_EXPORT void thistle_get_node_dimensions(const thistle_node_t* node, thistle_node_dimensions_t* dimensions)
{
    return;
}