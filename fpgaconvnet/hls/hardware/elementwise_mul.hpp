#ifndef ELEMENTWISE_MUL_HPP_
#define ELEMENTWISE_MUL_HPP_

#include "common.hpp"

/**
 * ELEMENTWISE MUL FUNCTION
 */
template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    typename elementwise_mul_t
>
void elementwise_mul(
    stream_t(elementwise_mul_t)  &in1,
    stream_t(elementwise_mul_t)  &in2,
    stream_t(elementwise_mul_t)  &out
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
        elementwise_mul_t tmp1 = in1.read();
        elementwise_mul_t tmp2 = in2.read();
        out.write(tmp1 * tmp2);
    }
}

#endif