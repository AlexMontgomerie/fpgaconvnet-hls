#include "conv_tb.hpp"
#include "conv.hpp"

void conv_top(
    stream_t(conv_data_t) in[CONV_KERNEL_SIZE_0][CONV_KERNEL_SIZE_1],
    conv_weight_t weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUPS)][CONV_KERNEL_SIZE_0][CONV_KERNEL_SIZE_1],
    stream_t(conv_acc_t) &out
)
{

#pragma HLS DATAFLOW
#pragma HLS RESOURCE variable=weights core=ROM_2P_BRAM

    conv<
#if CONV_BATCH_SIZE*CONV_ROWS*CONV_COLS > 1
        CONV_BATCH_SIZE,
        CONV_ROWS,
        CONV_COLS,
#endif
        CONV_CHANNELS,
#if CONV_FILTERS > 1
        CONV_FILTERS,
#endif
        CONV_GROUPS,
#if CONV_CHANNELS_PER_GROUP > 1
        CONV_CHANNELS_PER_GROUP,
#endif
#if CONV_FILTERS > 1
        CONV_FILTERS_PER_GROUP,
#endif
#if (CONV_KERNEL_SIZE_0 > 1) || (CONV_KERNEL_SIZE_1 > 1)
        CONV_FINE,
        CONV_KERNEL_SIZE_0,
        CONV_KERNEL_SIZE_1,
#endif
        conv_data_t,
        conv_weight_t,
        conv_acc_t
#if (CONV_FILTERS == 1) && (CONV_CHANNELS_PER_GROUP == 1)
        , conv_acc_t
#endif
#if (CONV_KERNEL_SIZE_0 == 1) && (CONV_KERNEL_SIZE_1 == 1)
    >(in[0][0],weights,out);
#else
    >(in,weights,out);
#endif

}
