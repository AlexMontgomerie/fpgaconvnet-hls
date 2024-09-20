#ifndef GLOBAL_POOL_TB_HPP_
#define GLOBAL_POOL_TB_HPP_

#include "common.hpp"
#include "global_pool_param.hpp"

// define the type based on the test configuration
typedef ap_fixed<GLOBAL_POOL_DATA_WIDTH, GLOBAL_POOL_DATA_INT_WIDTH, AP_RND,AP_SAT> global_pool_t;

void global_pool_top(
    stream_t(global_pool_t) &in, 
    stream_t(global_pool_t) &out
);

#endif 