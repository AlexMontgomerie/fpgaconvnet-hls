#include "fork_tb.hpp"
#include "fork.hpp"

void fork_top(
#if (FORK_KERNEL_SIZE_0 > 1) || (FORK_KERNEL_SIZE_1 > 1)
    stream_t(fork_t) in[FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1],
    stream_t(fork_t) out[FORK_COARSE][FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1]
#else
    stream_t(fork_t) &in,
    stream_t(fork_t) out[FORK_COARSE]
#endif
)
{

    #pragma HLS DATAFLOW

    fork<
        FORK_BATCH_SIZE,
        FORK_ROWS,
        FORK_COLS,
        FORK_CHANNELS,
        FORK_COARSE,
#if (FORK_KERNEL_SIZE_0 > 1) || (FORK_KERNEL_SIZE_1 > 1)
        FORK_KERNEL_SIZE_0,
        FORK_KERNEL_SIZE_1,
#endif
        fork_t
    >(in,out);

}
