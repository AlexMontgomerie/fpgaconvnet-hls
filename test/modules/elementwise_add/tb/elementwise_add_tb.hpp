#ifndef ELEMENTWISE_ADD_TB_HPP_
#define ELEMENTWISE_ADD_TB_HPP_

#include "common.hpp"
#include "elementwise_add_param.hpp"

// define the type based on the test configuration
typedef ap_fixed<ELEMENTWISE_ADD_DATA_WIDTH,ELEMENTWISE_ADD_DATA_INT_WIDTH,AP_RND,AP_SAT> elementwise_add_t;

void elementwise_add_top(
    stream_t(elementwise_add_t) &in1,
    stream_t(elementwise_add_t) &in2,
    stream_t(elementwise_add_t) &out
);

#endif