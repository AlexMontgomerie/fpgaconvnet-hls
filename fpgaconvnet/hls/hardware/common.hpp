#ifndef COMMON_HPP_
#define COMMON_HPP_

#include "system.hpp"
#include <ap_fixed.h>
#include "hls_stream.h"
#include "hls_math.h"
#include "ap_axi_sdata.h"
#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ERROR_TOLERANCE 0.20

// macro for template variable pragmas
#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)

// macro for integer division
#define DIVIDE(a,b) ((const unsigned int) ((a)/(b)))

// macro for evaluating min and max
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))

// macro for stream wrapper
#define stream_t(x) hls::stream<x>

// default data types
typedef unsigned long mem_int;
typedef ap_axis<64,1,1,1> axi_stream_t;
typedef hls::stream<axi_stream_t> axi_stream_hw_t;

typedef ap_fixed<16, 8,AP_RND> data_t;
typedef ap_fixed<30,16,AP_RND> acc_t;
typedef ap_fixed<16, 8,AP_RND> weight_t;

#endif
