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
#if ACCUM_BATCH_SIZE*ACCUM_ROWS*ACCUM_COLS*ACCUM_GROUPS > 1
        ACCUM_BATCH_SIZE,
        ACCUM_ROWS,
        ACCUM_COLS,
#endif
        ACCUM_CHANNELS,
        ACCUM_FILTERS,
#if ACCUM_FILTERS_PER_GROUP > 1
        ACCUM_FILTERS_PER_GROUP,
#endif
#if ACCUM_BATCH_SIZE*ACCUM_ROWS*ACCUM_COLS*ACCUM_GROUPS > 1
        ACCUM_GROUPS,
#endif
        test_accum_t
    >(in,out);

}

