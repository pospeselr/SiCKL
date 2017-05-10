#pragma once

struct thistle_node
{
    virtual void calc_output(
        const thistle_sample_batch_t* inputBatch,
        thistle_sample_batch_t* outputBatch) const = 0;
};
typedef thistle_node thistle_node_t;

