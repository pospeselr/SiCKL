#pragma once

struct thistle_label_node : public thistle_node
{

    // thistle_node interface
    size_t get_parameter_count() const override;
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
        thistle_buffer_t* inputDeltas) const override;

};