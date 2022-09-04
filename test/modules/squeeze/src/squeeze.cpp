#include "squeeze_tb.hpp"
#include "squeeze.hpp"

void squeeze_top(
    stream_t(squeeze_t) in[SQUEEZE_COARSE_IN],
    stream_t(squeeze_t) out[SQUEEZE_COARSE_OUT]
)
{

    #pragma HLS DATAFLOW
    squeeze<
        SQUEEZE_BATCH_SIZE,
        SQUEEZE_ROWS,
        SQUEEZE_COLS,
        SQUEEZE_CHANNELS,
        SQUEEZE_COARSE_IN,
        SQUEEZE_COARSE_OUT,
        squeeze_t
    >(in,out);

}
