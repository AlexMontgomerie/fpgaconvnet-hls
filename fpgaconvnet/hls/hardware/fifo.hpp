#ifndef FIFO_HPP_
#define FIFO_HPP_

#include "common.hpp"

template<
    unsigned int COARSE,
    unsigned int DEPTH
>
void fifo(
    stream_t(data_t) in[COARSE],
    stream_t(data_t) out[COARSE]
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

    const unsigned int coarse      = COARSE;
    const unsigned int fifo_depth  = DEPTH;

#pragma HLS STREAM variable=in depth=1
#pragma HLS STREAM variable=out depth=1

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    stream_t(data_t)  stream_fifo[coarse];
DO_PRAGMA(HLS STREAM variable=stream_fifo depth=fifo_depth)
#pragma HLS ARRAY_PARTITION variable=stream_fifo complete dim=0


    in_loop: for (unsigned int in_index = 0; in_index < coarse; in_index++) {
    #pragma HLS unroll
        stream_fifo[in_index].write(in[in_index].read());
    }

    out_loop: for (unsigned int out_index = 0; out_index < coarse; out_index++) {
    #pragma HLS unroll
        out[out_index].write(stream_fifo[out_index].read());
    }  
}

#endif
