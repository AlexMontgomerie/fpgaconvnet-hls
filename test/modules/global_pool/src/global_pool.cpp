#include "global_pool_tb.hpp"
#include "global_pool.hpp"

void global_pool_top(
    stream_t(global_pool_t) &in, 
    stream_t(global_pool_t) &out
)
{
#pragma HLS DATAFLOW

    global_pool<
        GLOBAL_POOL_BATCH_SIZE,
        GLOBAL_POOL_ROWS,
        GLOBAL_POOL_COLS,
        GLOBAL_POOL_CHANNELS,
        global_pool_t
    >(in,out);

}