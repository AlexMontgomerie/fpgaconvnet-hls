#ifndef ELEMENTWISE_MUL_TB_HPP_
#define ELEMENTWISE_MUL_TB_HPP_

#include "common.hpp"
#include "elementwise_mul_param.hpp"

// define the type based on the test configuration
typedef ap_fixed<ELEMENTWISE_MUL_DATA_WIDTH,ELEMENTWISE_MUL_DATA_INT_WIDTH,AP_RND,AP_SAT> elementwise_mul_t;

void elementwise_mul_top(
    stream_t(elementwise_mul_t) &in1,
    stream_t(elementwise_mul_t) &in2,
    stream_t(elementwise_mul_t) &out
);

#endif