#ifndef AVG_POOL_TB_HPP_
#define AVG_POOL_TB_HPP_

#include "common.hpp"
#include "avg_pool_param.hpp"

// define the type based on the test configuration
typedef ap_fixed<AVG_POOL_DATA_WIDTH, AVG_POOL_DATA_INT_WIDTH, AP_RND,AP_SAT> avg_pool_t;

void avg_pool_top(
    stream_t(avg_pool_t) in[AVG_POOL_KERNEL_SIZE_0][AVG_POOL_KERNEL_SIZE_1],
    stream_t(avg_pool_t) &out
);

#endif