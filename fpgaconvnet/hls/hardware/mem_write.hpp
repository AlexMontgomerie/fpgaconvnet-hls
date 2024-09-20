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

#ifndef MEM_WRITE_HPP_
#define MEM_WRITE_HPP_

#include "common.hpp"

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int PORTS,
    unsigned int STREAMS,
    unsigned int WEIGHTS_RELOADING_FACTOR,
    typename type_t,
    unsigned int DMA_WIDTH = 64,
    unsigned int DATA_WIDTH = 16,
    typename mem_t = mem_int
>
void mem_write(
    int weights_reloading_index,
    stream_t(type_t) out[STREAMS],
    volatile mem_t out_hw[PORTS][BATCH_SIZE*ROWS*COLS*DIVIDE(CHANNELS,STREAMS)*WEIGHTS_RELOADING_FACTOR]
)
{

#pragma HLS INLINE OFF

    const unsigned batch_size               = BATCH_SIZE;
    const unsigned rows                     = ROWS;
    const unsigned cols                     = COLS;
    const unsigned channels                 = CHANNELS;
    const unsigned ports                    = PORTS;
    const unsigned streams                  = STREAMS;
    const unsigned weights_reloading_factor = WEIGHTS_RELOADING_FACTOR;
    const unsigned channels_per_stream      = DIVIDE(channels,streams);
    const unsigned dma_channels             = DIVIDE(DMA_WIDTH,DATA_WIDTH);

    const unsigned dma_width = DMA_WIDTH;
    const unsigned data_width = DATA_WIDTH;
    const unsigned bit_mask = (1 << data_width) - 1;

#pragma HLS STREAM variable=out depth=256
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    // loops
    auto loops = hlslib::ConstFlatten<
        0, batch_size*rows*cols, 1, // pixel loop
        0, channels_per_stream, 1 // channel loop
    >();

    pixel_channel_loop: for (size_t i = 0; i < loops.size(); ++i, ++loops) {

        #pragma HLS PIPELINE II=1

        mem_int port_cache[ports] = {0};
        #pragma HLS dependence variable=port_cache intra RAW true

        // loop indices
        auto pixel_index = loops[0];
        auto channel_index = loops[1];

        streams_loop: for(unsigned stream_index=0; stream_index<streams; stream_index++) {
            type_t stream_cache = out[stream_index].read();
            unsigned int port_index = (int) (stream_index/dma_channels);
            type_t write_value; 
            write_value.range() = stream_cache.range() & bit_mask;
            port_cache[port_index] |= (stream_cache.range() & bit_mask) * mem_t(1) << (stream_index%dma_channels)*data_width;
        }

        port_write_loop: for (unsigned port_index=0; port_index < ports; port_index++) {
            int out_index = pixel_index*channels_per_stream*weights_reloading_factor +
                weights_reloading_index*channels_per_stream + channel_index;
            out_hw[port_index][out_index] = port_cache[port_index];
        }

    }

}

#endif
