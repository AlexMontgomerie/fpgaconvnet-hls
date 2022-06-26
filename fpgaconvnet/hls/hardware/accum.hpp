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
    unsigned int FILTERS_PER_GROUP,
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

    pixel_loop: for(unsigned int pixel_index=0; pixel_index<batch_size*rows*cols*channels; pixel_index++) {
        filter_loop: for(unsigned int filter_index=0; filter_index<filters_per_group; filter_index++) {
            #pragma HLS loop_flatten
            #pragma HLS pipeline II=1 rewind
            out[filter_index].write( in.read() );
        }
    }
}

/**
 *  accum reorder
 *  - single filter per group
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

    pixel_loop: for(unsigned int pixel_index=0; pixel_index<batch_size*rows*cols*channels; pixel_index++) {
        #pragma HLS pipeline II=1 rewind
        out[0].write( in.read() );
    }
}


template<
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int GROUPS,
    typename accum_t
>
void accum_accumulate_inner(
    stream_t(accum_t) in[DIVIDE(FILTERS, GROUPS)],
    stream_t(accum_t) &out,
    accum_t acc,
    unsigned int filter_index
) {

#pragma HLS INLINE OFF

    const unsigned int channels   = CHANNELS;
    const unsigned int filters    = FILTERS;
    const unsigned int groups     = GROUPS;
    const unsigned int channels_per_group = DIVIDE(channels,groups);
    const unsigned int filters_per_group  = DIVIDE(filters ,groups);

    channel_loop: for(unsigned int channel_index=0; channel_index<channels_per_group; channel_index++) {
        #pragma HLS loop_flatten
        #pragma HLS pipeline II=1 rewind
        accum_t cache = in[filter_index].read();
        acc = ( channel_index == 0 ) ?  cache : accum_t(cache + acc);
        if( channel_index == (channels_per_group-1) ) {
            out.write( acc ) ;
        }
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
    unsigned int FILTERS_PER_GROUP,
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

    pixel_loop: for(unsigned int pixel_index=0; pixel_index<batch_size*rows*cols*groups; pixel_index++) {
        filter_loop: for(unsigned int filter_index=0; filter_index<filters_per_group; filter_index++) {
            channel_loop: for(unsigned int channel_index=0; channel_index<channels_per_group; channel_index++) {
                #pragma HLS loop_flatten
                #pragma HLS pipeline II=1 rewind
                accum_t cache = in[filter_index].read();
                acc = ( channel_index == 0 ) ?  cache : accum_t(cache + acc);
                if( channel_index == (channels_per_group-1) ) {
                    out.write( acc ) ;
                }
            }
        }
    }
}

/**
 *  accum accumulate
 *  - single filter per group
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
    stream_t(accum_t) in[1],
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

    // accumulation cache
    accum_t acc = 0;
    #pragma HLS dependence variable=acc WAR intra false
    #pragma HLS dependence variable=acc RAW intra true

    pixel_loop: for(unsigned int pixel_index=0; pixel_index<batch_size*rows*cols*groups; pixel_index++) {
        channel_loop: for(unsigned int channel_index=0; channel_index<channels_per_group; channel_index++) {
            #pragma HLS loop_flatten
            #pragma HLS pipeline II=1 rewind
            accum_t cache = in[0].read();
            acc = ( channel_index == 0 ) ?  cache : accum_t(cache + acc);
            if( channel_index == (channels_per_group-1) ) {
                out.write( acc ) ;
            }
        }
    }
}


/**
 *  accum accumulate
 *  - single iteration
 */

template<
    unsigned int CHANNELS,
    unsigned int FILTERS,
    typename accum_t
>
void accum_accumulate(
    stream_t(accum_t) in[FILTERS],
    stream_t(accum_t) &out
)
{
    #pragma HLS INLINE OFF

    // get all constant parameters
    const unsigned int channels   = CHANNELS;
    const unsigned int filters    = FILTERS;
    const unsigned int filters_per_group  = filters;

    // accumulation cache
    accum_t acc = 0;
    #pragma HLS dependence variable=acc WAR intra false
    #pragma HLS dependence variable=acc RAW intra true

    filter_loop: for(unsigned int filter_index=0; filter_index<filters_per_group; filter_index++) {
        channel_loop: for(unsigned int channel_index=0; channel_index<channels; channel_index++) {
            #pragma HLS loop_flatten
            #pragma HLS pipeline II=1 rewind
            accum_t cache = in[filter_index].read();
            acc = ( channel_index == 0 ) ?  cache : accum_t(cache + acc);
            if( channel_index == (channels-1) ) {
                out.write( acc ) ;
            }
        }
    }
}

/**
 *  accum accumulate
 *  - single iteration
 *  - single filter per group
 */

template<
    unsigned int CHANNELS,
    typename accum_t
>
void accum_accumulate(
    stream_t(accum_t) in[1],
    stream_t(accum_t) &out
)
{
    #pragma HLS INLINE OFF

    const unsigned int channels   = CHANNELS;

    // accumulation cache
    accum_t acc = 0;
    #pragma HLS dependence variable=acc WAR intra false
    #pragma HLS dependence variable=acc RAW intra true

    channel_loop: for(unsigned int channel_index=0; channel_index<channels; channel_index++) {
        #pragma HLS loop_flatten
        #pragma HLS pipeline II=1 rewind
        accum_t cache = in[0].read();
        acc = ( channel_index == 0 ) ?  cache : accum_t(cache + acc);
        if( channel_index == (channels-1) ) {
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
    unsigned int FILTERS_PER_GROUP,
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

    #pragma HLS STREAM variable=in
    #pragma HLS STREAM variable=out

    // re-ordered stream
    stream_t(accum_t) reorder[filters_per_group];
    #pragma HLS STREAM variable=reorder
    #pragma HLS array_partition variable=reorder complete dim=0
    DO_PRAGMA(HLS STREAM variable=reorder depth=channels_per_group+1)

    #pragma HLS DATAFLOW
    accum_reorder<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        FILTERS,
        FILTERS_PER_GROUP,
        GROUPS,
        accum_t
    >(in, reorder);

    accum_accumulate<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        FILTERS,
        FILTERS_PER_GROUP,
        GROUPS,
        accum_t
    >(reorder, out);

}

/**
 *  accum
 *  - single filter per group
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
    const unsigned int filters_per_group  = filters;
    const unsigned int channels_per_group = DIVIDE(channels, groups);

    #pragma HLS STREAM variable=in
    #pragma HLS STREAM variable=out

    // re-ordered stream
    stream_t(accum_t) reorder[filters_per_group];
    #pragma HLS STREAM variable=reorder
    #pragma HLS array_partition variable=reorder complete dim=0
    DO_PRAGMA(HLS STREAM variable=reorder depth=channels_per_group+1)

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

/**
 *  accum
 *  - single iteration
 */

template<
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int FILTERS_PER_GROUP,
    typename accum_t
>
void accum(
    stream_t(accum_t) &in,
    stream_t(accum_t) &out
)
{

    #pragma HLS INLINE OFF

    // get all constant parameters
    const unsigned int filters_per_group  = FILTERS;
    const unsigned int channels = CHANNELS;

    #pragma HLS STREAM variable=in
    #pragma HLS STREAM variable=out

    // re-ordered stream
    stream_t(accum_t) reorder[filters_per_group];
    #pragma HLS STREAM variable=reorder
    #pragma HLS array_partition variable=reorder complete dim=0
    DO_PRAGMA(HLS STREAM variable=reorder depth=channels+1)

    #pragma HLS DATAFLOW
    accum_reorder<
        1,
        1,
        1,
        CHANNELS,
        FILTERS,
        FILTERS,
        1,
        accum_t
    >(in, reorder);

    accum_accumulate<
        CHANNELS,
        FILTERS,
        accum_t
    >(reorder, out);

}

/**
 *  accum
 *  - single iteration
 *  - single filter
 */

template<
    unsigned int CHANNELS,
    unsigned int FILTERS,
    typename accum_t
>
void accum(
    stream_t(accum_t) &in,
    stream_t(accum_t) &out
)
{

    #pragma HLS INLINE OFF

    #pragma HLS STREAM variable=in
    #pragma HLS STREAM variable=out

    const unsigned int channels = CHANNELS;

    // re-ordered stream
    stream_t(accum_t) reorder[1];
    #pragma HLS stream variable=reorder
    #pragma HLS array_partition variable=reorder complete dim=0
    DO_PRAGMA(HLS STREAM variable=reorder depth=channels+1)

    #pragma HLS DATAFLOW
    accum_reorder<
        1,
        1,
        1,
        CHANNELS,
        1,
        1,
        accum_t
    >(in, reorder);

    accum_accumulate<
        CHANNELS,
        accum_t
    >(reorder, out);

}

#endif
