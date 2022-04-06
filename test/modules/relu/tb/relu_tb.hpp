#ifndef RELU_TB_HPP_
#define RELU_TB_HPP_

#include "common.hpp"
#include "relu_param.hpp"

// define the type based on the test configuration
typedef ap_fixed<RELU_DATA_WIDTH,RELU_DATA_INT_WIDTH,AP_RND,AP_SAT> relu_t;

void relu_top(
	stream_t(relu_t) &in,
	stream_t(relu_t) &out
);

#endif
