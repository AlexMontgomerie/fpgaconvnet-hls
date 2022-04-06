#ifndef ACCUM_TB_HPP_
#define ACCUM_TB_HPP_

#include "common.hpp"
#include "accum_param.hpp"

// define the type based on the test configuration
typedef ap_fixed<ACCUM_DATA_WIDTH, ACCUM_DATA_INT_WIDTH> test_accum_t;

void accum_top(
	stream_t(test_accum_t) &in,
	stream_t(test_accum_t) &out
);

#endif
