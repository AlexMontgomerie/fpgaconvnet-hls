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

#ifndef MEM_READ_HPP_
#define MEM_READ_HPP_

#include "common.hpp"

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int PORTS,
    unsigned int STREAMS,
    typename type_t,
    unsigned int DMA_WIDTH = 64,
    unsigned int DATA_WIDTH = 16,
    typename mem_t = mem_int
>
void mem_read(
    volatile mem_t in_hw[PORTS][BATCH_SIZE*ROWS*COLS*DIVIDE(CHANNELS,STREAMS)],
    stream_t(type_t) in[STREAMS]
)
{

#pragma HLS INLINE OFF

    const unsigned batch_size           = BATCH_SIZE;
    const unsigned rows                 = ROWS;
    const unsigned cols                 = COLS;
    const unsigned channels             = CHANNELS;
    const unsigned ports                = PORTS;
    const unsigned streams              = STREAMS;
    const unsigned channels_per_stream  = DIVIDE(channels,streams);
    const unsigned dma_channels         = DIVIDE(DMA_WIDTH, DATA_WIDTH);

    const unsigned dma_width = DMA_WIDTH;
    const unsigned data_width = DATA_WIDTH;
    const unsigned bit_mask = (1 << data_width) - 1;

#pragma HLS STREAM variable=in depth=256
#pragma HLS ARRAY_PARTITION variable=in complete dim=0

    read_loop: for (unsigned long size_index=0; size_index < batch_size*rows*cols*channels_per_stream; size_index++) {
        #pragma HLS PIPELINE II=1

        mem_t port_cache[ports];
        #pragma HLS ARRAY_PARTITION variable=port_cache complete dim=0

        port_read_loop: for (unsigned port_index=0; port_index < ports; port_index++) {
            port_cache[port_index] = in_hw[port_index][size_index];
        }

        stream_loop: for (unsigned int stream_index=0; stream_index < streams; stream_index++) {

            // get the port index
            unsigned int port_index = (int)(stream_index/dma_channels);

            // get the stream value
            type_t stream_cache = 0;
            stream_cache.range() = ( ( ( port_cache[port_index] ) >>
                        ( ( stream_index%dma_channels ) * data_width ) ) & bit_mask );

            // write to the stream
            in[stream_index].write(stream_cache);

        }
    }
}

#endif
