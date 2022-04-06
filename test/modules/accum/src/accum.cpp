#include "accum_tb.hpp"
#include "accum.hpp"

void accum_top(
	stream_t(test_accum_t) &in,
	stream_t(test_accum_t) &out
)
{

    #pragma HLS INTERFACE axis port=in
    #pragma HLS INTERFACE axis port=out

    #pragma HLS DATAFLOW

    // DUT
    accum<
        ACCUM_BATCH_SIZE,
        ACCUM_ROWS,
        ACCUM_COLS,
        ACCUM_CHANNELS,
        ACCUM_FILTERS,
        ACCUM_GROUPS,
        test_accum_t
    >(in,out);

}

