#include "elementwise_add_tb.hpp"
#include "elementwise_add.hpp"

void elementwise_add_top(
    stream_t(elementwise_add_t) &in1,
    stream_t(elementwise_add_t) &in2,
    stream_t(elementwise_add_t) &out
)
{

#pragma HLS DATAFLOW

    // DUT
    elementwise_add<
        ELEMENTWISE_ADD_BATCH_SIZE,
        ELEMENTWISE_ADD_ROWS,
        ELEMENTWISE_ADD_COLS,
        ELEMENTWISE_ADD_CHANNELS,
        elementwise_add_t
    >(in1,in2,out);

}
