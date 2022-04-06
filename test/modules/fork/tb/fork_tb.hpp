#ifndef FORK_TB_HPP_
#define FORK_TB_HPP_

#include "common.hpp"
#include "fork_param.hpp"

// define the type based on the test configuration
typedef ap_fixed<FORK_DATA_WIDTH, FORK_DATA_INT_WIDTH, AP_RND,AP_SAT> fork_t;

void fork_top(
#if (FORK_KERNEL_SIZE_0 > 1) || (FORK_KERNEL_SIZE_1 > 1)
    stream_t(fork_t) in[FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1],
    stream_t(fork_t) out[FORK_COARSE][FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1]
#else
    stream_t(fork_t) &in,
    stream_t(fork_t) out[FORK_COARSE]
#endif
);

#endif
