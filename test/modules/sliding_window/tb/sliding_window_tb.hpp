#ifndef SLIDING_WINDOW_TB_HPP_
#define SLIDING_WINDOW_TB_HPP_

#include "common.hpp"
#include "sliding_window_param.hpp"

// define the type based on the test configuration
typedef ap_fixed<SLIDING_WINDOW_DATA_WIDTH,SLIDING_WINDOW_DATA_INT_WIDTH,AP_RND,AP_SAT> sliding_window_t;

void sliding_window_top(
    stream_t(sliding_window_t) &in,
    stream_t(sliding_window_t) out[SLIDING_WINDOW_KERNEL_SIZE_0][SLIDING_WINDOW_KERNEL_SIZE_1]
);

//////////////////////////////////////////

#endif
