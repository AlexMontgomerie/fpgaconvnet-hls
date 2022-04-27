#ifndef BIAS_TB_HPP_
#define BIAS_TB_HPP_

#include "common.hpp"
#include "bias_param.hpp"

typedef     ap_fixed<BIAS_DATA_WIDTH,BIAS_DATA_INT_WIDTH,AP_RND> bias_data_t;
typedef     ap_fixed<BIAS_BIASES_WIDTH,BIAS_BIASES_INT_WIDTH,AP_RND> bias_biases_t;

void bias_top(
    stream_t(bias_data_t) &in,
    bias_biases_t biases[BIAS_FILTERS],
    stream_t(bias_data_t) &out
);

#endif
