#include "thistle.hpp"

#include "thistle_error.hpp"
#include "thistle_buffer.hpp"
#include "thistle_node.hpp"
#include "thistle_label_node.hpp"

void thistle_label_node::calc_output(
    const thistle_buffer_t* inputBatch,
    const thistle_buffer_t* constants,
    thistle_buffer_t* outputBatch) const
{

}

void thistle_label_node::calc_parameter_deltas(
    const thistle_buffer_t* inputBatch,
    const thistle_buffer_t* outputBatchDelta,
    const thistle_buffer_t* constants,
    thistle_buffer_t* parameterDeltas) const
{

}

void thistle_label_node::calc_input_deltas(
    const thistle_buffer_t* inputBatch,
    const thistle_buffer_t* outputBatchDelta,
    const thistle_buffer_t* constants,
    thistle_buffer_t* inputDeltas) const
{

}