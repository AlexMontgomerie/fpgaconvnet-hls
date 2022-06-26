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

#ifndef WR_HPP_
#define WR_HPP_

#include "common.hpp"

/**
 *  WEIGHTS RELOADING FUNCTION
 */

template<
unsigned int FILTERS,
unsigned int COARSE_IN,
unsigned int COARSE_OUT,
unsigned int COARSE_GROUP,
unsigned int KERNEL_SIZE_X,
unsigned int KERNEL_SIZE_Y,
typename weight_t
>
void weights_reloading(
    stream_t(weight_t) &in,
    weight_t weights[COARSE_IN*COARSE_GROUP][COARSE_OUT][DIVIDE(FILTERS,COARSE_IN*COARSE_GROUP*COARSE_OUT*KERNEL_SIZE_X*KERNEL_SIZE_Y)][KERNEL_SIZE_X][KERNEL_SIZE_Y]
)
{

#pragma HLS INLINE OFF
#pragma HLS STREAM variable=in

    const unsigned int coarse_in     = COARSE_IN;
    const unsigned int coarse_out    = COARSE_OUT;
    const unsigned int coarse_group  = COARSE_GROUP;
    const unsigned int filters       = DIVIDE(FILTERS,COARSE_IN*COARSE_GROUP*COARSE_OUT*KERNEL_SIZE_X*KERNEL_SIZE_Y);
    const unsigned int kernel_size_x = KERNEL_SIZE_X;
    const unsigned int kernel_size_y = KERNEL_SIZE_Y;

    coarse_in_loop: for(unsigned int coarse_in_index=0; coarse_in_index < coarse_in*coarse_group; coarse_in_index++) {
        coarse_out_loop: for(unsigned int coarse_out_index=0; coarse_out_index < coarse_out; coarse_out_index++) {
            filter_loop: for(unsigned int filter_index=0; filter_index < filters; filter_index++) {
                kernel_1_loop: for(unsigned int k1_index=0; k1_index < kernel_size_x; k1_index++) {
                    kernel_2_loop: for(unsigned int k2_index=0; k2_index < kernel_size_y; k2_index++) {
                        #pragma HLS PIPELINE II=1
                        #pragma HLS dependence variable=weights inter false
                        weights[coarse_in_index][coarse_out_index][filter_index][k1_index][k2_index] = in.read();
                    }
                }
            }
        }
    }
}

#endif
