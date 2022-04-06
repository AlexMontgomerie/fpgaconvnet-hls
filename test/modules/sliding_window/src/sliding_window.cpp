#include "sliding_window_tb.hpp"
#include "sliding_window.hpp"

void sliding_window_top(
    stream_t(sliding_window_t) &in,
    stream_t(sliding_window_t) out[SLIDING_WINDOW_KERNEL_SIZE_0][SLIDING_WINDOW_KERNEL_SIZE_1]
)
{
    #pragma HLS ARRAY_PARTITION variable=out complete dim=0
    #pragma HLS DATAFLOW

#if SLIDING_WINDOW_KERNEL_SIZE_0 == 1
    sliding_window_1d_horizontal<
        SLIDING_WINDOW_BATCH_SIZE,
        SLIDING_WINDOW_ROWS,
        SLIDING_WINDOW_COLS,
        SLIDING_WINDOW_CHANNELS,
        SLIDING_WINDOW_PAD_TOP,
        SLIDING_WINDOW_PAD_RIGHT,
        SLIDING_WINDOW_PAD_BOTTOM,
        SLIDING_WINDOW_PAD_LEFT,
        SLIDING_WINDOW_STRIDE_0,
        SLIDING_WINDOW_STRIDE_1,
        SLIDING_WINDOW_KERNEL_SIZE_1,
        sliding_window_t
    >(in,out);
#elif  SLIDING_WINDOW_KERNEL_SIZE_1 == 1
    sliding_window_1d_vertical<
        SLIDING_WINDOW_BATCH_SIZE,
        SLIDING_WINDOW_ROWS,
        SLIDING_WINDOW_COLS,
        SLIDING_WINDOW_CHANNELS,
        SLIDING_WINDOW_PAD_TOP,
        SLIDING_WINDOW_PAD_RIGHT,
        SLIDING_WINDOW_PAD_BOTTOM,
        SLIDING_WINDOW_PAD_LEFT,
        SLIDING_WINDOW_STRIDE_0,
        SLIDING_WINDOW_STRIDE_1,
        SLIDING_WINDOW_KERNEL_SIZE_0,
        sliding_window_t
    >(in,out);
#else
    sliding_window<
        SLIDING_WINDOW_BATCH_SIZE,
        SLIDING_WINDOW_ROWS,
        SLIDING_WINDOW_COLS,
        SLIDING_WINDOW_CHANNELS,
        SLIDING_WINDOW_PAD_TOP,
        SLIDING_WINDOW_PAD_RIGHT,
        SLIDING_WINDOW_PAD_BOTTOM,
        SLIDING_WINDOW_PAD_LEFT,
        SLIDING_WINDOW_STRIDE_0,
        SLIDING_WINDOW_STRIDE_1,
        SLIDING_WINDOW_KERNEL_SIZE_0,
        SLIDING_WINDOW_KERNEL_SIZE_1,
        sliding_window_t
    >(in,out);
#endif

}
