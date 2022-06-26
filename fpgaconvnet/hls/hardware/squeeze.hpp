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
    unsigned int CHANNELS_PER_COARSE,
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

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    dim_in_loop: for (unsigned int pixel_index = 0; pixel_index < batch_size*rows*cols; pixel_index++) {
        unsigned int cache_index = 0;
        channel_in_loop: for (unsigned int channel_index = 0; channel_index < DIVIDE(channels,coarse); channel_index++) {
            #pragma HLS loop_flatten
            #pragma HLS pipeline II=1 rewind
            for (unsigned int in_index = 0; in_index < coarse; in_index++) {
                out[cache_index].write(in[in_index].read());
                cache_index++;
            }
        }
    }
}

/**
 *  squeeze in
 *  - single channel per coarse
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

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    dim_in_loop: for (unsigned int pixel_index = 0; pixel_index < batch_size*rows*cols; pixel_index++) {
        unsigned int cache_index = 0;
        #pragma HLS pipeline II=1 rewind
        for (unsigned int in_index = 0; in_index < coarse; in_index++) {
            out[cache_index].write(in[in_index].read());
            cache_index++;
        }
    }
}

/**
 *  squeeze in
 *  - single iteration
 */
template<
    unsigned int CHANNELS,
    unsigned int CHANNELS_PER_COARSE,
    unsigned int COARSE,
    typename squeeze_t
>
void squeeze_in(
    stream_t(squeeze_t) in[COARSE],
    stream_t(squeeze_t) out[CHANNELS]
)
{

#pragma HLS INLINE OFF

    const unsigned int channels     = CHANNELS;
    const unsigned int coarse       = COARSE;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    unsigned int cache_index = 0;
    channel_in_loop: for (unsigned int channel_index = 0; channel_index < DIVIDE(channels,coarse); channel_index++) {
        #pragma HLS loop_flatten
        #pragma HLS pipeline II=1 rewind
        for (unsigned int in_index = 0; in_index < coarse; in_index++) {
            out[cache_index].write(in[in_index].read());
            cache_index++;
        }
    }
}

/**
 *  squeeze in
 *  - single iteration
 *  - single channel per coarse
 */
template<
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

    const unsigned int channels     = CHANNELS;
    const unsigned int coarse       = COARSE;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    unsigned int cache_index = 0;

    #pragma HLS unroll
    for (unsigned int in_index = 0; in_index < coarse; in_index++) {
        out[cache_index].write(in[in_index].read());
        cache_index++;
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
    unsigned int CHANNELS_PER_COARSE,
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

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    dim_out_loop: for (unsigned int pixel_index = 0; pixel_index < batch_size*rows*cols; pixel_index++) {
        unsigned int cache_index = 0;
        channel_out_loop: for (unsigned int channel_index = 0; channel_index < DIVIDE(channels,coarse); channel_index++) {
            #pragma HLS loop_flatten
            #pragma HLS pipeline II=1 rewind
            for (unsigned int out_index = 0; out_index < coarse; out_index++) {
                out[out_index].write(in[cache_index].read());
                cache_index++;
            }
        }
    }
}

/**
 *  squeeze out
 *  - single channel per coarse
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

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    dim_out_loop: for (unsigned int pixel_index = 0; pixel_index < batch_size*rows*cols; pixel_index++) {
        unsigned int cache_index = 0;
        #pragma HLS pipeline II=1 rewind
        for (unsigned int out_index = 0; out_index < coarse; out_index++) {
            out[out_index].write(in[cache_index].read());
            cache_index++;
        }
    }
}

/**
 *  squeeze out
 *  - single iteration
 */
template<
    unsigned int CHANNELS,
    unsigned int CHANNELS_PER_COARSE,
    unsigned int COARSE,
    typename squeeze_t
>
void squeeze_out(
    stream_t(squeeze_t) in[CHANNELS],
    stream_t(squeeze_t) out[COARSE]
)
{

#pragma HLS INLINE OFF

    const unsigned int channels     = CHANNELS;
    const unsigned int coarse       = COARSE;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    unsigned int cache_index = 0;
    channel_out_loop: for (unsigned int channel_index = 0; channel_index < DIVIDE(channels,coarse); channel_index++) {
        #pragma HLS pipeline II=1 rewind
        for (unsigned int out_index = 0; out_index < coarse; out_index++) {
            out[out_index].write(in[cache_index].read());
            cache_index++;
        }
    }
}

/**
 *  squeeze out
 *  - single iteration
 *  - single channel per coarse
 */
template<
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

    const unsigned int channels     = CHANNELS;
    const unsigned int coarse       = COARSE;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

#pragma HLS unroll

    unsigned int cache_index = 0;

    for (unsigned int out_index = 0; out_index < coarse; out_index++) {
        out[out_index].write(in[cache_index].read());
        cache_index++;
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
    unsigned int CHANNELS_PER_COARSE_IN,
    unsigned int CHANNELS_PER_COARSE_IN_DUP,
    unsigned int CHANNELS_PER_COARSE_OUT,
    typename squeeze_t
>
void squeeze_spatial(
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
        CHANNELS_PER_COARSE_IN,
        COARSE_IN,
        squeeze_t
    >(in, cache);

    squeeze_out<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        CHANNELS_PER_COARSE_OUT,
        COARSE_OUT,
        squeeze_t
    >(cache, out);

}

/**
 *  squeeze
 *  - single channel per coarse in
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int COARSE_IN,
    unsigned int COARSE_OUT,
    unsigned int CHANNELS_PER_COARSE_OUT,
    typename squeeze_t
>
void squeeze_spatial(
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
        CHANNELS_PER_COARSE_OUT,
        COARSE_OUT,
        squeeze_t
    >(cache, out);

}

/**
 *  squeeze
 *  - single channel per coarse out
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int COARSE_IN,
    unsigned int COARSE_OUT,
    unsigned int CHANNELS_PER_COARSE_IN,
    unsigned int CHANNELS_PER_COARSE_IN_DUP,
    typename squeeze_t
>
void squeeze_spatial(
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
        CHANNELS_PER_COARSE_IN,
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

/**
 *  squeeze
 *  - single channel per coarse in
 *  - single channel per coarse out
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
void squeeze_spatial(
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

/**
 *  squeeze
 *  - single iteration
 */

template<
    unsigned int CHANNELS,
    unsigned int COARSE_IN,
    unsigned int COARSE_OUT,
    unsigned int CHANNELS_PER_COARSE_IN,
    unsigned int CHANNELS_PER_COARSE_IN_DUP,
    unsigned int CHANNELS_PER_COARSE_OUT,
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
        CHANNELS,
        CHANNELS_PER_COARSE_IN,
        COARSE_IN,
        squeeze_t
    >(in, cache);

    squeeze_out<
        CHANNELS,
        CHANNELS_PER_COARSE_OUT,
        COARSE_OUT,
        squeeze_t
    >(cache, out);

}

/**
 *  squeeze
 *  - single iteration
 *  - single channel per coarse in
 */

template<
    unsigned int CHANNELS,
    unsigned int COARSE_IN,
    unsigned int COARSE_OUT,
    unsigned int CHANNELS_PER_COARSE_OUT,
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
        CHANNELS,
        COARSE_IN,
        squeeze_t
    >(in, cache);

    squeeze_out<
        CHANNELS,
        CHANNELS_PER_COARSE_OUT,
        COARSE_OUT,
        squeeze_t
    >(cache, out);

}

/**
 *  squeeze
 *  - single iteration
 *  - single channel per coarse out
 */

template<
    unsigned int CHANNELS,
    unsigned int COARSE_IN,
    unsigned int COARSE_OUT,
    unsigned int CHANNELS_PER_COARSE_IN,
    unsigned int CHANNELS_PER_COARSE_IN_DUP,
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
        CHANNELS,
        CHANNELS_PER_COARSE_IN,
        COARSE_IN,
        squeeze_t
    >(in, cache);

    squeeze_out<
        CHANNELS,
        COARSE_OUT,
        squeeze_t
    >(cache, out);

}

/**
 *  squeeze
 *  - single iteration
 *  - single channel per coarse in
 *  - single channel per coarse out
 */

template<
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
        CHANNELS,
        COARSE_IN,
        squeeze_t
    >(in, cache);

    squeeze_out<
        CHANNELS,
        COARSE_OUT,
        squeeze_t
    >(cache, out);

}
#endif
