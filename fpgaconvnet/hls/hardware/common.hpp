/**
 * Copyright (C) 2022 Alexander Montgomerie-Corcoran
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <ap_int.h>
#include <ap_fixed.h>
#include "hls_stream.h"
#include "hls_math.h"
#include "ap_axi_sdata.h"
#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef SYSTEM_HPP_
#include "system.hpp"
#endif

// hlslib imports
#include "hlslib/xilinx/Flatten.h"

#define ERROR_TOLERANCE 0.2

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
/* #define stream_t(x) hlslib::Stream<x> */

// default data types
typedef ap_uint<64> mem_int;
typedef ap_axis<64,1,1,1> axi_stream_t;
typedef hls::stream<axi_stream_t> axi_stream_hw_t;

// <total_width, int_width, rounding type>
typedef ap_fixed<16, 8,AP_RND> data_t;
typedef ap_fixed<30,16,AP_RND> acc_t;
typedef ap_fixed<16, 8,AP_RND> weight_t;

#endif
