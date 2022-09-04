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
        CONV_BATCH_SIZE,
        CONV_ROWS,
        CONV_COLS,
        CONV_CHANNELS,
        CONV_FILTERS,
        CONV_GROUPS,
#if (CONV_KERNEL_SIZE_0 > 1) || (CONV_KERNEL_SIZE_1 > 1)
        CONV_FINE,
        CONV_KERNEL_SIZE_0,
        CONV_KERNEL_SIZE_1,
#endif
        conv_data_t,
        conv_weight_t,
        conv_acc_t
#if (CONV_KERNEL_SIZE_0 == 1) && (CONV_KERNEL_SIZE_1 == 1)
    >(in[0][0],weights,out);
#else
    >(in,weights,out);
#endif

}
