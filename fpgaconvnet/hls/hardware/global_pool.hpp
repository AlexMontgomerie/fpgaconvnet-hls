#ifndef GLOBAL_POOL_HPP_
#define GLOBAL_POOL_HPP_

#include "common.hpp"
/**
  * GLOBAL_POOL
  */

 template <
     unsigned int BATCH_SIZE,
     unsigned int ROWS,
     unsigned int COLS,
     unsigned int CHANNELS,
     typename global_pool_t
>

void global_pool(
    stream_t(global_pool_t) &in, 
    stream_t(global_pool_t) &out
)
{
#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS; 

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    global_pool_t pool_cache[rows*cols][batch_size*channels];
    #pragma HLS ARRAY_PARTITION variable=pool_cache complete dim=0
    #pragma HLS DEPENDENCE variable=pool_cache RAW intra true

    ap_fixed<64, 56, AP_RND,AP_SAT> accum_value; 
    global_pool_t accum_cache; 

    read_loop: for (unsigned long i = 0; i < rows*cols; i++) {
        read_loop2: for (unsigned long j = 0; j < batch_size*channels; j++) {
            pool_cache[i][j] = in.read();
            // std::cout << "pool_cache[" << i << "][" << j << "] = " << pool_cache[i][j] << std::endl;
        }
    }

    average_loop: for (unsigned long i = 0; i < batch_size*channels; i++) {
        #pragma HLS PIPELINE II=1 rewind
        accum_value = 0;        

        average_loop2: for (unsigned long j = 0; j < rows*cols; j++) {
            accum_value += pool_cache[j][i];
            // std::cout << "adding pool_cache[" << j << "][" << i << "] = " << pool_cache[j][i] << std::endl;
        }
        // std::cout << "rows*cols = " << rows*cols << std::endl;
        accum_value = accum_value / (rows*cols);
        accum_cache.range() = (accum_value.range(15,8) << 8) + (accum_value.range(7,0));
        // std::cout << "accum_value_range = " << accum_value.range() << ", accum_cache_range = " << accum_cache.range() << std::endl;
        out.write(accum_cache);
    }
}

#endif