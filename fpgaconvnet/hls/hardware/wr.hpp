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
    const unsigned int filters       = DIVIDE(FILTERS, COARSE_IN*COARSE_GROUP*COARSE_OUT*KERNEL_SIZE_X*KERNEL_SIZE_Y);
    const unsigned int kernel_size_x = KERNEL_SIZE_X;
    const unsigned int kernel_size_y = KERNEL_SIZE_Y;

    // loops
    auto loops = hlslib::ConstFlatten<
        0, coarse_in*coarse_group, 1, // coarse in loop
        0, coarse_out, 1, // coarse out loop
        0, filters, 1, // filter loop
        0, kernel_size_x, 1, // k1 loop
        0, kernel_size_y, 1 // k1 loop
    >();

    coarse_filter_kernel_loop: for (size_t i = 0; i < loops.size(); ++i, ++loops) {

        // pragmas
        #pragma HLS PIPELINE II=1
        #pragma HLS dependence variable=weights inter false

        // loop indices
        auto coarse_in_index = loops[0];
        auto coarse_out_index = loops[1];
        auto filter_index = loops[2];
        auto k1_index = loops[3];
        auto k2_index = loops[4];

        // convert to weights stream
        weights[coarse_in_index][coarse_out_index][filter_index][k1_index][k2_index] = in.read();

    }

}

#endif
