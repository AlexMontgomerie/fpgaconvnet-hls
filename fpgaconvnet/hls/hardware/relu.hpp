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

#ifndef RELU_HPP_
#define RELU_HPP_

#include "common.hpp"

/**
 *  RECTIFIED LINEAR UNIT (RELU) FUNCTION
 */
template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    typename relu_t
>
void relu(
    stream_t(relu_t)  &in,
    stream_t(relu_t)  &out
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    for(unsigned long pixel_index=0 ; pixel_index < batch_size*rows*cols*channels ; pixel_index++) {
        #pragma HLS PIPELINE II=1 rewind
        relu_t tmp = in.read();
        if(tmp < 0.0)
            out.write(0.0);
        else
            out.write(tmp);
    }
}

#endif
