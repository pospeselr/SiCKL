#pragma once

struct thistle_node
{
    virtual size_t get_parameter_count() const = 0;

    virtual void calc_output(
        const thistle_buffer_t* inputBatch,
        thistle_buffer_t* outputBatch) const = 0;

    virtual void calc_parameter_deltas(
        const thistle_buffer_t* inputBatch,
        const thistle_buffer_t* outputBatchDeltas,
        thistle_buffer_t* parameterDeltas) const = 0;
};
typedef thistle_node thistle_node_t;

