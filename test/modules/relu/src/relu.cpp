#include "relu_tb.hpp"
#include "relu.hpp"

void relu_top(
    stream_t(relu_t) &in,
    stream_t(relu_t) &out
)
{

#pragma HLS DATAFLOW

    // DUT
    relu<
        RELU_BATCH_SIZE,
        RELU_ROWS,
        RELU_COLS,
        RELU_CHANNELS,
        relu_t
    >(in,out);

}
