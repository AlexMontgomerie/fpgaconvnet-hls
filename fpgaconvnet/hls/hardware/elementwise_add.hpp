#ifndef ELEMENTWISE_ADD_HPP_
#define ELEMENTWISE_ADD_HPP_

#include "common.hpp"

/**
 * ELEMENTWISE ADD FUNCTION
 */
template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    typename elementwise_add_t
>
void elementwise_add(
    stream_t(elementwise_add_t)  &in1,
    stream_t(elementwise_add_t)  &in2,
    stream_t(elementwise_add_t)  &out
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;

#pragma HLS STREAM variable=in1
#pragma HLS STREAM variable=in2
#pragma HLS STREAM variable=out

    for(unsigned long pixel_index=0 ; pixel_index < batch_size*rows*cols*channels ; pixel_index++) {
        #pragma HLS PIPELINE II=1 rewind
        elementwise_add_t tmp1 = in1.read();
        elementwise_add_t tmp2 = in2.read();
        out.write(tmp1 + tmp2);
    }
}

#endif
