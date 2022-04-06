#ifndef POOL_TB_HPP_
#define POOL_TB_HPP_

#include "common.hpp"
#include "pool_param.hpp"

// define the type based on the test configuration
typedef ap_fixed<POOL_DATA_WIDTH, POOL_DATA_INT_WIDTH, AP_RND,AP_SAT> pool_t;

void pool_top(
	stream_t(pool_t) in[POOL_KERNEL_SIZE_0][POOL_KERNEL_SIZE_1],
	stream_t(pool_t) &out
);


#endif
