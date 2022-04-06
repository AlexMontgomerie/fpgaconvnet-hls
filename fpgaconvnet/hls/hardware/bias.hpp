#include "common.hpp"

/**
 *  BIAS FUNCTION
 */

/*
template<int _>
void bias(
    stream_t(data_t) &in,
    data_t bias[CHANNELS],
    stream_t(data_t) &out
)
{

#pragma HLS INLINE OFF 

    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int coarse       = NAME_SUB(MODULE_NAME,_COARSE);

#pragma HLS STREAM variable=in 
#pragma HLS STREAM variable=out

#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1)
    pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS) != 1
        channel_loop: for(unsigned int channel_index=0;channel_index<channels;channel_ndex++) {
#else
            unsigned int channel_index=0;
#endif
            #pragma HLS PIPELINE II=1 rewind
	    out.write( in.read() + bias[channel_index] );
#if NAME_SUB(MODULE_NAME,_CHANNELS) != 1
	}
#endif
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1)
    }
#endif
}
*/
