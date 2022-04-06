#ifndef BATCH_NORM_HPP_ 
#define BATCH_NORM_HPP_ 

#include "common.hpp"

/**
 *  BATCH NORMALIZATION + SCALE FUNCTION
 */

template<int _>
void NAME_SUB(name,_batch_norm)(
    stream_t(data_t) &in,
    const data_t scale[NAME_SUB(MODULE_NAME,_CHANNELS)],  
    const data_t shift[NAME_SUB(MODULE_NAME,_CHANNELS)],  
    stream_t(data_t) &out
)
{

#pragma HLS INLINE OFF 

    const unsigned batch_size = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned rows       = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned cols       = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned channels   = NAME_SUB(MODULE_NAME,_CHANNELS);
 
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    pixel_loop: for(unsigned pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
#if NAME_SUB(MODULE_NAME,_CHANNELS) != 1
        channel_loop: for(int channel_index=0;channel_index<channels;channel_index++) {
#pragma HLS PIPELINE II=1 rewind
            out.write( ( in.read() + shift[channel_index] ) * scale[channel_index] );
        }
#else
#pragma HLS PIPELINE II=1 rewind
        out.write( ( in.read() + shift[0] ) * scale[0] );
#endif
    }
}

#endif
