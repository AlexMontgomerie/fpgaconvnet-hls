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

#ifndef CONV_HPP_
#define CONV_HPP_

#include "common.hpp"

/**
 *  conv intr
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int GROUPS,
    unsigned int FINE,
    unsigned int KERNEL_SIZE_X,
    unsigned int KERNEL_SIZE_Y,
    typename conv_data_t,
    typename conv_weight_t
>
void conv_intr(
    stream_t(conv_data_t)    in[KERNEL_SIZE_X][KERNEL_SIZE_Y],
    const conv_weight_t      weights[DIVIDE(CHANNELS*FILTERS,GROUPS)][KERNEL_SIZE_X][KERNEL_SIZE_Y],
    stream_t(conv_data_t)    window_stream[FINE],
    stream_t(conv_weight_t)  weight_stream[FINE]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size    = BATCH_SIZE;
    const unsigned int rows          = ROWS;
    const unsigned int cols          = COLS;
    const unsigned int channels      = CHANNELS;
    const unsigned int filters       = FILTERS;
    const unsigned int groups        = GROUPS;
    const unsigned int kernel_size_x = KERNEL_SIZE_X;
    const unsigned int kernel_size_y = KERNEL_SIZE_Y;
    const unsigned int fine          = FINE;
    const unsigned int interval      = DIVIDE(kernel_size_x*kernel_size_y,fine);

    const unsigned int channels_per_group = DIVIDE(channels,groups);
    const unsigned int filters_per_group  = DIVIDE(filters ,groups);

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=window_stream
#pragma HLS STREAM variable=weight_stream

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=window_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=weight_stream complete dim=0

    // partition the weights correctly
    const unsigned int weights_partition_factor_k1 = MIN(fine,kernel_size_x);
    const unsigned int weights_partition_factor_k2 = (fine<=kernel_size_x) ? 1 : kernel_size_y;

    DO_PRAGMA(HLS ARRAY_PARTITION variable=weights block factor=weights_partition_factor_k1 dim=2)
    DO_PRAGMA(HLS ARRAY_PARTITION variable=weights block factor=weights_partition_factor_k2 dim=3)

    // window cache
    conv_data_t window_cache[kernel_size_x][kernel_size_y];
    #pragma HLS ARRAY_PARTITION variable=window_cache complete dim=0
    #pragma HLS dependence variable=window_cache intra RAW true
    #pragma HLS dependence variable=window_cache inter WAW false
    /* DO_PRAGMA( HLS dependence variable=window_cache inter WAW true distance=batch_size*rows*cols*channels ) */

    // loops
    auto loops = hlslib::ConstFlatten<
        0, batch_size*rows*cols, 1, // pixel loop
        0, channels_per_group, 1, // channel loop
        0, filters, 1 // filter loop
    >();

    // extra indices
    unsigned int weight_index = 0;

    pixel_channel_filter_loop: for (size_t i = 0; i < loops.size(); ++i, ++loops) {

        // pragmas
        DO_PRAGMA( HLS PIPELINE II=interval rewind)

        // loop indices
        auto channel_index = loops[1];
        auto filter_index = loops[2];
        unsigned char fine_index = 0;

        // perform the interleaving
        intr_k2_loop: for(unsigned char k2=0; k2 < kernel_size_y; k2++) {
            intr_k1_loop: for(unsigned char k1=0; k1 < kernel_size_x; k1++) {

                // read into the window cache
                if(filter_index % filters_per_group == 0) {
                    window_cache[k1][k2] = in[k1][k2].read();
                }

                // write to the window and weights streams
                window_stream[fine_index].write(window_cache[k1][k2]);
                weight_stream[fine_index].write(weights[weight_index][k1][k2]);

                // increment the fine index
                fine_index = ( fine_index + 1 ) % fine;
            }
        }

        // weight index logic
        weight_index = (weight_index+1) % (channels_per_group*filters);

    }

}

/**
 *  conv mul
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int GROUPS,
    unsigned int FINE,
    unsigned int KERNEL_SIZE_X,
    unsigned int KERNEL_SIZE_Y,
    typename conv_data_t,
    typename conv_weight_t,
    typename conv_acc_t
>
void conv_mul(
    stream_t(conv_data_t) window_stream[FINE],
    stream_t(conv_weight_t) weight_stream[FINE],
    stream_t(conv_acc_t) acc_stream[FINE]
)
{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=window_stream
#pragma HLS STREAM variable=weight_stream
#pragma HLS STREAM variable=acc_stream

    const unsigned int batch_size    = BATCH_SIZE;
    const unsigned int rows          = ROWS;
    const unsigned int cols          = COLS;
    const unsigned int channels      = CHANNELS;
    const unsigned int filters       = FILTERS;
    const unsigned int groups        = GROUPS;
    const unsigned int kernel_size_x = KERNEL_SIZE_X;
    const unsigned int kernel_size_y = KERNEL_SIZE_Y;
    const unsigned int fine          = FINE;
    const unsigned int interval      = DIVIDE(kernel_size_x*kernel_size_y,fine);


    const unsigned int channels_per_group = DIVIDE(channels,groups);
    const unsigned int filters_per_group  = DIVIDE(filters ,groups);

#pragma HLS ARRAY_PARTITION variable=window_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=weight_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=acc_stream    complete dim=0

    // MULTIPLICATION LOOP
    conv_acc_t acc_cache[fine];
    unsigned char acc_index=0;
    mul_pixel_loop: for(unsigned int pixel_index=0; pixel_index <
                            batch_size*rows*cols*channels_per_group*filters_per_group*groups*interval; pixel_index++) {
            #pragma HLS pipeline II=1
            mul_loop: for(unsigned char fine_index=0; fine_index < fine; fine_index++) {

                // update accumulation cache
                conv_acc_t prev = ( acc_index == 0 ) ? conv_acc_t(0) : acc_cache[fine_index] ;
                acc_cache[fine_index] = prev + window_stream[fine_index].read() * weight_stream[fine_index].read();

                // write to output stream
                if( acc_index == (interval-1) ) {
                    acc_stream[fine_index].write( acc_cache[fine_index] ) ;
                }
            }

            // increment accumulation index
            acc_index = (acc_index+1) % interval;
    }
}

/**
 *  conv acc
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int GROUPS,
    unsigned int FINE,
    unsigned int KERNEL_SIZE_X,
    unsigned int KERNEL_SIZE_Y,
    typename conv_acc_t
>
void conv_acc(
    stream_t(conv_acc_t) acc_stream[FINE],
    stream_t(conv_acc_t) &out
)
{

    #pragma HLS INLINE OFF

    #pragma HLS STREAM variable=acc_stream
    #pragma HLS ARRAY_PARTITION variable=acc_stream complete dim=0

    #pragma HLS STREAM variable=out

    const unsigned int batch_size    = BATCH_SIZE;
    const unsigned int rows          = ROWS;
    const unsigned int cols          = COLS;
    const unsigned int channels      = CHANNELS;
    const unsigned int filters       = FILTERS;
    const unsigned int groups        = GROUPS;
    const unsigned int kernel_size_x = KERNEL_SIZE_X;
    const unsigned int kernel_size_y = KERNEL_SIZE_Y;
    const unsigned int fine          = FINE;

    const unsigned int channels_per_group = DIVIDE(channels,groups);
    const unsigned int filters_per_group  = DIVIDE(filters ,groups);

    // ACCUMULATION LOOP
    acc_pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols*channels_per_group*filters_per_group*groups;pixel_index++) {
        #pragma HLS pipeline II=1 rewind
        conv_acc_t acc = 0 ;
        acc_fine_loop: for(unsigned char fine_index=0;fine_index<fine;fine_index++) {
            acc += acc_stream[fine_index].read();
        }
        out.write(acc);
    }
}

/**
 *  conv
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int GROUPS,
    unsigned int FINE,
    unsigned int KERNEL_SIZE_X,
    unsigned int KERNEL_SIZE_Y,
    typename conv_data_t,
    typename conv_weight_t,
    typename conv_acc_t
>
void conv(
    stream_t(conv_data_t) in[KERNEL_SIZE_X][KERNEL_SIZE_Y],
    const conv_weight_t weights[DIVIDE(CHANNELS*FILTERS,GROUPS)][KERNEL_SIZE_X][KERNEL_SIZE_Y],
    stream_t(conv_acc_t) &out
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0

    const unsigned int fine = FINE;

    stream_t(conv_data_t) window_stream[fine];
    stream_t(conv_weight_t) weight_stream[fine];
    stream_t(conv_acc_t) acc_stream[fine];

    #pragma HLS STREAM variable=window_stream
    #pragma HLS STREAM variable=weight_stream
    #pragma HLS STREAM variable=acc_stream

    conv_intr<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        FILTERS,
        GROUPS,
        FINE,
        KERNEL_SIZE_X,
        KERNEL_SIZE_Y,
        conv_data_t,
        conv_weight_t
    >(in,weights,window_stream,weight_stream);

    conv_mul<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        FILTERS,
        GROUPS,
        FINE,
        KERNEL_SIZE_X,
        KERNEL_SIZE_Y,
        conv_data_t,
        conv_weight_t,
        conv_acc_t
    >(window_stream,weight_stream,acc_stream);

    conv_acc<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        FILTERS,
        GROUPS,
        FINE,
        KERNEL_SIZE_X,
        KERNEL_SIZE_Y,
        conv_acc_t
    >(acc_stream, out);

}

/**
 *  point-wise convolution
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int GROUPS,
    typename conv_data_t,
    typename conv_weight_t,
    typename conv_acc_t
>
void conv(
    stream_t(conv_data_t) &in,
    const conv_weight_t weights[DIVIDE(CHANNELS*FILTERS,GROUPS)][1][1],
    stream_t(conv_acc_t) &out
)
{

#pragma HLS INLINE OFF

    const unsigned batch_size   = BATCH_SIZE;
    const unsigned rows         = ROWS;
    const unsigned cols         = COLS;
    const unsigned channels     = CHANNELS;
    const unsigned filters      = FILTERS;
    const unsigned groups       = GROUPS;

    const unsigned int channels_per_group = DIVIDE(channels,groups);
    const unsigned int filters_per_group  = DIVIDE(filters ,groups);

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    // cache for the incoming sample
    conv_data_t window_cache;
    // result for the dot product
    conv_acc_t dot;

    // loops
    auto loops = hlslib::ConstFlatten<
        0, batch_size*rows*cols, 1, // pixel loop
        0, channels_per_group, 1, // channel loop
        0, filters, 1 // filter loop
    >();

    // extra indices
    unsigned int weight_index = 0;

    pixel_channel_filter_loop: for (size_t i = 0; i < loops.size(); ++i, ++loops) {

        // pragma
        #pragma HLS pipeline II=1 rewind

        // loop indices
        auto channel_index = loops[1];
        auto filter_index = loops[2];

        // update the cache
        if(filter_index%filters_per_group == 0) {
            window_cache = in.read();
        }

        // perform the dot product
        dot = window_cache * weights[weight_index][0][0];

        // write to the dotproduct to the output
        out.write(dot);

        // weight index logic
        weight_index = (weight_index+1) % (channels_per_group*filters);

    }

}

#endif
