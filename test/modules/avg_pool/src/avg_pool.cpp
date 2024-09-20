#include "avg_pool_tb.hpp"
#include "avg_pool.hpp"

void avg_pool_top(
    stream_t(avg_pool_t) in[AVG_POOL_KERNEL_SIZE_0][AVG_POOL_KERNEL_SIZE_1],
    stream_t(avg_pool_t) &out
)
{

#pragma HLS DATAFLOW

    avg_pool<
        AVG_POOL_BATCH_SIZE,
        AVG_POOL_ROWS,
        AVG_POOL_COLS,
        AVG_POOL_CHANNELS,
        AVG_POOL_KERNEL_SIZE_0,
        AVG_POOL_KERNEL_SIZE_1,
        avg_pool_t
    >(in,out);

}