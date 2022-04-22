#ifndef MEM_WRITE_TB_HPP_
#define MEM_WRITE_TB_HPP_

#include "common.hpp"

#include "mem_write_param.hpp"

#define MEM_WRITE_DMA_WIDTH  64
#define MEM_WRITE_DATA_WIDTH 16
#define MEM_WRITE_BIT_MASK   ((1<<(MEM_WRITE_DATA_WIDTH))-1)

/////////////////////////////////

void mem_write_top(
    int weights_reloading_index,
    stream_t(data_t) out[MEM_WRITE_STREAMS_OUT],
    volatile mem_int out_hw[MEM_WRITE_PORTS_OUT][MEM_WRITE_BATCH_SIZE*MEM_WRITE_ROWS_OUT*MEM_WRITE_COLS_OUT*DIVIDE(MEM_WRITE_CHANNELS_OUT,MEM_WRITE_STREAMS_OUT)*MEM_WRITE_WEIGHTS_RELOADING_FACTOR]
);

/////////////////////////////////

#endif
