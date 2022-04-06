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
    unsigned int BUFFER_SIZE,
    typename squeeze_t
>
void squeeze_in(
    stream_t(squeeze_t) in[COARSE],
    stream_t(squeeze_t) out[BUFFER_SIZE]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
    const unsigned int coarse       = COARSE;
    const unsigned int buffer_size  = BUFFER_SIZE;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    unsigned int cache_index = 0;

    dim_in_loop: for (unsigned int pixel_index = 0; pixel_index < batch_size*rows*cols*DIVIDE(channels,coarse); pixel_index++) {
        #pragma HLS pipeline II=1 rewind
        for (unsigned int in_index = 0; in_index < coarse; in_index++) {
            out[cache_index].write(in[in_index].read());
            cache_index++;
#ifndef __SYNTHESIS__
            cache_index = cache_index % buffer_size;
#endif
        }
    }
}

/*
 * squeeze in
 * - single iteration
 */

template<
    unsigned int CHANNELS,
    unsigned int COARSE,
    unsigned int BUFFER_SIZE,
    typename squeeze_t
>
void squeeze_in(
    stream_t(squeeze_t) in[COARSE],
    stream_t(squeeze_t) out[BUFFER_SIZE]
)
{

#pragma HLS INLINE OFF

    const unsigned int coarse       = COARSE;
    const unsigned int buffer_size  = BUFFER_SIZE;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    unsigned int cache_index = 0;

    #pragma HLS unroll region
    for (unsigned int in_index = 0; in_index < coarse; in_index++) {
        out[cache_index].write(in[in_index].read());
        cache_index++;
#ifndef __SYNTHESIS__
        cache_index = cache_index % buffer_size;
#endif
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
    unsigned int BUFFER_SIZE,
    typename squeeze_t
>
void squeeze_out(
    stream_t(squeeze_t) in[BUFFER_SIZE],
    stream_t(squeeze_t) out[COARSE]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
    const unsigned int coarse       = COARSE;
    const unsigned int buffer_size  = BUFFER_SIZE;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    unsigned int cache_index = 0;

    dim_out_loop: for (unsigned int pixel_index = 0; pixel_index < batch_size*rows*cols*DIVIDE(channels,coarse); pixel_index++) {
        #pragma HLS pipeline II=1 rewind
        for (unsigned int out_index = 0; out_index < coarse; out_index++) {
            out[out_index].write(in[cache_index].read());
            cache_index++;
#ifndef __SYNTHESIS__
            cache_index = cache_index % buffer_size;
#endif
        }
    }
}

/**
 *  squeeze out
 *  - single iteration
 */

template<
    unsigned int CHANNELS,
    unsigned int COARSE,
    unsigned int BUFFER_SIZE,
    typename squeeze_t
>
void squeeze_out(
    stream_t(squeeze_t) in[BUFFER_SIZE],
    stream_t(squeeze_t) out[COARSE]
)
{

#pragma HLS INLINE OFF

    const unsigned int channels     = CHANNELS;
    const unsigned int coarse       = COARSE;
    const unsigned int buffer_size  = BUFFER_SIZE;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    unsigned int cache_index = 0;

    #pragma HLS unroll region
    for (unsigned int out_index = 0; out_index < coarse; out_index++) {
        out[out_index].write(in[cache_index].read());
        cache_index++;
#ifndef __SYNTHESIS__
        cache_index = cache_index % buffer_size;
#endif
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
    unsigned int BUFFER_SIZE,
    typename squeeze_t
>
void squeeze(
    stream_t(squeeze_t) in[COARSE_IN],
    stream_t(squeeze_t) out[COARSE_OUT]
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

    const unsigned int buffer_size  = BUFFER_SIZE;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    stream_t(squeeze_t)  cache[buffer_size];
#pragma HLS STREAM variable=cache
#pragma HLS ARRAY_PARTITION variable=cache complete dim=0

    squeeze_in<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        COARSE_IN,
        BUFFER_SIZE,
        squeeze_t
    >(in, cache);

    squeeze_out<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        COARSE_OUT,
        BUFFER_SIZE,
        squeeze_t
    >(cache, out);

}

/**
 *  squeeze
 *  - squeeze in single iteration
 */

template<
    unsigned int CHANNELS,
    unsigned int COARSE_IN,
    unsigned int COARSE_OUT,
    unsigned int BUFFER_SIZE,
    typename squeeze_t
>
void squeeze(
    stream_t(squeeze_t) in[COARSE_IN],
    stream_t(squeeze_t) out[COARSE_OUT]
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

    const unsigned int buffer_size  = BUFFER_SIZE;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    stream_t(squeeze_t)  cache[buffer_size];
#pragma HLS STREAM variable=cache
#pragma HLS ARRAY_PARTITION variable=cache complete dim=0

    squeeze_in<
        CHANNELS,
        COARSE_IN,
        BUFFER_SIZE,
        squeeze_t
    >(in, cache);

    squeeze_out<
        1,
        1,
        1,
        CHANNELS,
        COARSE_OUT,
        BUFFER_SIZE,
        squeeze_t
    >(cache, out);

}

/**
 *  squeeze
 *  - squeeze out single iteration
 */

template<
    unsigned int CHANNELS,
    unsigned int COARSE_IN,
    unsigned int COARSE_OUT,
    unsigned int CHANNELS_PER_COARSE_IN,
    unsigned int BUFFER_SIZE,
    typename squeeze_t
>
void squeeze(
    stream_t(squeeze_t) in[COARSE_IN],
    stream_t(squeeze_t) out[COARSE_OUT]
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

    const unsigned int buffer_size  = BUFFER_SIZE;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    stream_t(squeeze_t)  cache[buffer_size];
#pragma HLS STREAM variable=cache
#pragma HLS ARRAY_PARTITION variable=cache complete dim=0

    squeeze_in<
        1,
        1,
        1,
        CHANNELS,
        COARSE_IN,
        BUFFER_SIZE,
        squeeze_t
    >(in, cache);

    squeeze_out<
        CHANNELS,
        COARSE_OUT,
        BUFFER_SIZE,
        squeeze_t
    >(cache, out);

}

#endif
