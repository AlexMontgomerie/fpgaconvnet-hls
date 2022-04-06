#ifndef CONV_TB_HPP_
#define CONV_TB_HPP_

#include "common.hpp"
#include "conv_param.hpp"

typedef     ap_fixed<CONV_DATA_WIDTH,CONV_DATA_INT_WIDTH,AP_RND,AP_SAT> conv_data_t;
typedef     ap_fixed<CONV_WEIGHT_WIDTH,CONV_WEIGHT_INT_WIDTH,AP_RND,AP_SAT> conv_weight_t;
typedef     ap_fixed<CONV_ACC_WIDTH,CONV_ACC_INT_WIDTH,AP_RND,AP_SAT> conv_acc_t;

void conv_top(
    stream_t(conv_data_t) in[CONV_KERNEL_SIZE_0][CONV_KERNEL_SIZE_1],
    conv_weight_t weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUPS)][CONV_KERNEL_SIZE_0][CONV_KERNEL_SIZE_1],
    stream_t(conv_acc_t) &out
);

#endif
