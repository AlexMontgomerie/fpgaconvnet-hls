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

#ifndef FORK_HPP_
#define FORK_HPP_

#include "common.hpp"

/**
 *  FORK FUNCTION
 */
template<
unsigned int BATCH_SIZE,
unsigned int ROWS,
unsigned int COLS,
unsigned int CHANNELS,
unsigned int COARSE,
typename fork_t
>
void fork(
    stream_t(fork_t) &in,
    stream_t(fork_t) out[COARSE]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
    const unsigned int coarse       = COARSE;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    fork_t local_cache;
#pragma HLS DEPENDENCE variable=local_cache RAW intra true
#pragma HLS DEPENDENCE variable=local_cache inter false

    pixel_loop: for (unsigned long pixel_index = 0; pixel_index < batch_size*rows*cols*channels; pixel_index++) {
        #pragma HLS PIPELINE II=1 rewind
        coarse_loop: for(unsigned int coarse_index=0;coarse_index < coarse; coarse_index++) {
            if(coarse_index == 0) {
                DO_PRAGMA(HLS occurrence cycle=batch_size*rows*cols*channels)
                local_cache = in.read();
            }
            out[coarse_index].write(local_cache);
        }
    }
}

/**
 *  FORK FUNCTION
 */
template<
unsigned int BATCH_SIZE,
unsigned int ROWS,
unsigned int COLS,
unsigned int CHANNELS,
unsigned int COARSE,
unsigned int KERNEL_SIZE_X,
unsigned int KERNEL_SIZE_Y,
typename fork_t
>
void fork(
    stream_t(fork_t) in[KERNEL_SIZE_X][KERNEL_SIZE_Y],
    stream_t(fork_t) out[COARSE][KERNEL_SIZE_X][KERNEL_SIZE_Y]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size    = BATCH_SIZE;
    const unsigned int rows          = ROWS;
    const unsigned int cols          = COLS;
    const unsigned int channels      = CHANNELS;
    const unsigned int coarse        = COARSE;
    const unsigned int kernel_size_x = KERNEL_SIZE_X;
    const unsigned int kernel_size_y = KERNEL_SIZE_Y;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    fork_t local_cache[kernel_size_x][kernel_size_y];
#pragma HLS ARRAY_PARTITION variable=local_cache complete dim=0
#pragma HLS DEPENDENCE variable=local_cache RAW intra true
#pragma HLS DEPENDENCE variable=local_cache inter false

    pixel_loop: for (unsigned long pixel_index = 0; pixel_index < batch_size*rows*cols*channels; pixel_index++) {
        #pragma HLS PIPELINE II=1 rewind
        k1_loop: for (unsigned char k1 = 0; k1 < kernel_size_x; k1++) {
            k2_loop: for (unsigned char k2 = 0; k2 < kernel_size_y; k2++) {
                coarse_loop: for(unsigned int coarse_index=0;coarse_index < coarse; coarse_index++) {
                    if(coarse_index == 0) {
                        DO_PRAGMA(HLS occurrence cycle=batch_size*rows*cols*channels*kernel_size_x*kernel_size_y)
                        local_cache[k1][k2] = in[k1][k2].read();
                    }
                    out[coarse_index][k1][k2].write(local_cache[k1][k2]);
                }
            }
        }
    }
}

#endif
