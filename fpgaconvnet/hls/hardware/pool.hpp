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

#ifndef POOL_HPP_
#define POOL_HPP_

#include "common.hpp"

/**
  * POOL
  */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int KERNEL_SIZE_X,
    unsigned int KERNEL_SIZE_Y,
    typename pool_t
>
void pool(
    stream_t(pool_t) in[KERNEL_SIZE_X][KERNEL_SIZE_Y],
    stream_t(pool_t) &out
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
    const unsigned int kernel_size_x = KERNEL_SIZE_X;
    const unsigned int kernel_size_y = KERNEL_SIZE_Y;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out
#pragma HLS ARRAY_PARTITION variable=in complete dim=0

    pool_t cache;
    #pragma HLS DEPENDENCE variable=cache RAW intra true

    pixel_loop: for(unsigned long pixel_index=0;pixel_index<batch_size*rows*cols*channels;pixel_index++) {
        #pragma HLS PIPELINE II=1 rewind

        pool_loop_1: for (unsigned char k1 = 0; k1 < kernel_size_x; k1++) {
            pool_loop_2: for (unsigned char k2 = 0; k2 < kernel_size_y; k2++) {
                if (k1 == 0 && k2 == 0) {
                    cache = in[k1][k2].read();
                }
                else {
                    pool_t tmp = in[k1][k2].read();
                    cache = (cache > tmp ) ? cache : tmp ;
                }
            }
        }

        out.write(cache);

    }
}

#endif
