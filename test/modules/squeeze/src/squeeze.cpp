#include "squeeze_tb.hpp"
#include "squeeze.hpp"

void squeeze_top(
    stream_t(squeeze_t) in[SQUEEZE_COARSE_IN],
    stream_t(squeeze_t) out[SQUEEZE_COARSE_OUT]
)
{

    #pragma HLS DATAFLOW
    squeeze<
#if ( SQUEEZE_BATCH_SIZE*SQUEEZE_COLS*SQUEEZE_ROWS*SQUEEZE_CHANNELS_PER_COARSE_IN > 1 ) && ( SQUEEZE_BATCH_SIZE*SQUEEZE_COLS*SQUEEZE_ROWS*SQUEEZE_CHANNELS_PER_COARSE_OUT > 1 )
        SQUEEZE_BATCH_SIZE,
        SQUEEZE_ROWS,
        SQUEEZE_COLS,
#endif
        SQUEEZE_CHANNELS,
        SQUEEZE_COARSE_IN,
        SQUEEZE_COARSE_OUT,
#if SQUEEZE_BATCH_SIZE*SQUEEZE_COLS*SQUEEZE_ROWS*SQUEEZE_CHANNELS_PER_COARSE_IN > 1
        SQUEEZE_CHANNELS_PER_COARSE_IN,
#endif
        SQUEEZE_BUFFER_SIZE,
        squeeze_t
    >(in,out);

}
