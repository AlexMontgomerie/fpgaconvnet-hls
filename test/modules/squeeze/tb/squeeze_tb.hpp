#ifndef SQUEEZE_TB_HPP_
#define SQUEEZE_TB_HPP_

#include "common.hpp"
#include "squeeze_param.hpp"

// define the type based on the test configuration
typedef ap_fixed<SQUEEZE_DATA_WIDTH,SQUEEZE_DATA_INT_WIDTH,AP_RND,AP_SAT> squeeze_t;

void squeeze_top(
    stream_t(squeeze_t) in[SQUEEZE_COARSE_IN],
    stream_t(squeeze_t) out[SQUEEZE_COARSE_OUT]
);

#endif
