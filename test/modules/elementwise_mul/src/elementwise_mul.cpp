#include "elementwise_mul_tb.hpp"
#include "elementwise_mul.hpp"

void elementwise_mul_top(
    stream_t(elementwise_mul_t) &in1,
    stream_t(elementwise_mul_t) &in2,
    stream_t(elementwise_mul_t) &out
)
{

#pragma HLS DATAFLOW

    // DUT
    elementwise_mul<
        ELEMENTWISE_MUL_BATCH_SIZE,
        ELEMENTWISE_MUL_ROWS,
        ELEMENTWISE_MUL_COLS,
        ELEMENTWISE_MUL_CHANNELS,
        elementwise_mul_t
    >(in1,in2,out);

}