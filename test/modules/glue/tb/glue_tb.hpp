#ifndef GLUE_TB_HPP_
#define GLUE_TB_HPP_

#include "common.hpp"
#include "glue_param.hpp"

// define the type based on the test configuration
typedef ap_fixed<GLUE_DATA_WIDTH, GLUE_DATA_INT_WIDTH, AP_RND,AP_SAT> glue_data_t;
typedef ap_fixed<GLUE_ACC_WIDTH, GLUE_ACC_INT_WIDTH, AP_RND,AP_SAT> glue_acc_t;

void glue_top(
    stream_t(glue_acc_t) in[GLUE_COARSE_IN][GLUE_COARSE_OUT],
    stream_t(glue_data_t) out[GLUE_COARSE_OUT]
);

#endif
