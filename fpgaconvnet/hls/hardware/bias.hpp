/**
 * Copyright (C) 2022 Alexander Montgomerie-Corcoran and Benjamin Biggs
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

#ifndef BIAS_HPP_
#define BIAS_HPP_

#include "common.hpp"

/**
 *  BIAS FUNCTION
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int FILTERS,
    typename bias_data_t,
    typename bias_biases_t
>
void bias(
    stream_t(bias_data_t) &in,
    const bias_biases_t bias[FILTERS],
    stream_t(bias_data_t) &out
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int filters      = FILTERS;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    // loops
    auto loops = hlslib::ConstFlatten<
        0, batch_size*rows*cols, 1, // pixel loop
        0, filters, 1 // filter loop
    >();

    pixel_filter_loop: for (size_t i = 0; i < loops.size(); ++i, ++loops) {
        #pragma HLS PIPELINE II=1 rewind
        auto filter_index = loops[1];
        out.write(in.read() + bias[filter_index]);
    }

}

#endif
