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

#ifndef ACCUM_HPP_
#define ACCUM_HPP_

#include "common.hpp"

/**
 *  accum reorder
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int GROUPS,
    typename accum_t
>
void accum_reorder(
    stream_t(accum_t) &in,
    stream_t(accum_t) out[DIVIDE(FILTERS, GROUPS)]
)
{
    #pragma HLS INLINE OFF

    // get all constant parameters
    const unsigned int batch_size = BATCH_SIZE;
    const unsigned int rows       = ROWS;
    const unsigned int cols       = COLS;
    const unsigned int channels   = CHANNELS;
    const unsigned int filters    = FILTERS;
    const unsigned int groups     = GROUPS;
    const unsigned int channels_per_group = DIVIDE(channels,groups);
    const unsigned int filters_per_group  = DIVIDE(filters ,groups);

    // loops
    auto loops = hlslib::ConstFlatten<
        0, batch_size*rows*cols*channels, 1, // pixel loop
        0, filters_per_group, 1 // filter loop
    >();

    pixel_filter_loop: for (size_t i = 0; i < loops.size(); ++i, ++loops) {
        #pragma HLS pipeline II=1 rewind
        auto filter_index = loops[1];
        out[filter_index].write( in.read() );
    }

 }

/**
 *  accum accumulate
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int GROUPS,
    typename accum_t
>
void accum_accumulate(
    stream_t(accum_t) in[DIVIDE(FILTERS, GROUPS)],
    stream_t(accum_t) &out
)
{
    #pragma HLS INLINE OFF

    // get all constant parameters
    const unsigned int batch_size = BATCH_SIZE;
    const unsigned int rows       = ROWS;
    const unsigned int cols       = COLS;
    const unsigned int channels   = CHANNELS;
    const unsigned int filters    = FILTERS;
    const unsigned int groups     = GROUPS;
    const unsigned int channels_per_group = DIVIDE(channels,groups);
    const unsigned int filters_per_group  = DIVIDE(filters ,groups);

    // accumulation cache
    accum_t acc = 0;
    #pragma HLS dependence variable=acc WAR intra false
    #pragma HLS dependence variable=acc RAW intra true

    // loops
    auto loops = hlslib::ConstFlatten<
        0, batch_size*rows*cols*groups, 1,
        0, filters_per_group, 1,
        0, channels_per_group, 1
    >();

    pixel_filter_channel_loop: for (size_t i = 0; i < loops.size(); ++i, ++loops) {
        #pragma HLS pipeline II=1 rewind

        // loop indices
        auto filter_index = loops[1];
        auto channel_index = loops[2];

        accum_t cache = in[filter_index].read();
        acc = ( channel_index == 0 ) ?  cache : accum_t(cache + acc);
        if( channel_index == (channels_per_group-1) ) {
            out.write( acc ) ;
        }

    }

}

/**
 *  accum
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int GROUPS,
    typename accum_t
>
void accum(
    stream_t(accum_t) &in,
    stream_t(accum_t) &out
)
{

    #pragma HLS INLINE OFF

    // get all constant parameters
    const unsigned int channels   = CHANNELS;
    const unsigned int filters    = FILTERS;
    const unsigned int groups     = GROUPS;
    const unsigned int filters_per_group  = DIVIDE(filters, groups);
    const unsigned int channels_per_group = DIVIDE(channels, groups);

    // pipeline depth (from synthesis)
    const unsigned pipeline_depth = 10;

    #pragma HLS STREAM variable=in
    #pragma HLS STREAM variable=out

    // re-ordered stream
    stream_t(accum_t) reorder[filters_per_group];
    #pragma HLS STREAM variable=reorder
    #pragma HLS array_partition variable=reorder complete dim=0
    DO_PRAGMA(HLS STREAM variable=reorder depth=channels_per_group+pipeline_depth)

    #pragma HLS DATAFLOW
    accum_reorder<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        FILTERS,
        GROUPS,
        accum_t
    >(in, reorder);

    accum_accumulate<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        FILTERS,
        GROUPS,
        accum_t
    >(reorder, out);

}

#endif
