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

#ifndef SQUEEZE_HPP_
#define SQUEEZE_HPP_

#include "common.hpp"

/**
 *  squeeze in
 */
template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int COARSE,
    typename squeeze_t
>
void squeeze_in(
    stream_t(squeeze_t) in[COARSE],
    stream_t(squeeze_t) out[CHANNELS]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
    const unsigned int coarse       = COARSE;

    const unsigned int channels_per_coarse = DIVIDE(channels, coarse);

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    // loops
    auto loops = hlslib::ConstFlatten<
        0, batch_size*rows*cols, 1, // pixel loop
        0, channels_per_coarse, 1 // channel loop
    >();

    unsigned int cache_index = 0;

    pixel_channel_loop: for (size_t i = 0; i < loops.size(); ++i, ++loops) {

        // pragma
        #pragma HLS pipeline II=1 rewind

        // write to intermediate output stream
        for (unsigned int in_index = 0; in_index < coarse; in_index++) {
            out[cache_index+in_index].write(in[in_index].read());
        }

        // cache index logic
        cache_index = (cache_index + coarse) % channels;

    }

}

/**
 *  squeeze out
 */
template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int COARSE,
    typename squeeze_t
>
void squeeze_out(
    stream_t(squeeze_t) in[CHANNELS],
    stream_t(squeeze_t) out[COARSE]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
    const unsigned int coarse       = COARSE;

    const unsigned int channels_per_coarse = DIVIDE(channels, coarse);

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    // loops
    auto loops = hlslib::ConstFlatten<
        0, batch_size*rows*cols, 1, // pixel loop
        0, channels_per_coarse, 1 // channel loop
    >();

    unsigned int cache_index = 0;

    pixel_channel_loop: for (size_t i = 0; i < loops.size(); ++i, ++loops) {

        // pragma
        #pragma HLS pipeline II=1 rewind

        // write to intermediate output stream
        for (unsigned int out_index = 0; out_index < coarse; out_index++) {
            out[out_index].write(in[cache_index+out_index].read());
        }

        // cache index logic
        cache_index = (cache_index + coarse) % channels;

    }

}


/**
 *  squeeze
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int COARSE_IN,
    unsigned int COARSE_OUT,
    typename squeeze_t
>
void squeeze(
    stream_t(squeeze_t) in[COARSE_IN],
    stream_t(squeeze_t) out[COARSE_OUT]
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

    const unsigned int channels = CHANNELS;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    stream_t(squeeze_t)  cache[channels];
#pragma HLS STREAM variable=cache
#pragma HLS ARRAY_PARTITION variable=cache complete dim=0

    squeeze_in<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        COARSE_IN,
        squeeze_t
    >(in, cache);

    squeeze_out<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        COARSE_OUT,
        squeeze_t
    >(cache, out);

}

#endif
