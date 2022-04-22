#include "mem_write_tb.hpp"
#include "mem_write.hpp"

void mem_write_top(
    int weights_reloading_index,
    stream_t(data_t) out[MEM_WRITE_STREAMS_OUT],
    volatile mem_int out_hw[MEM_WRITE_PORTS_OUT][MEM_WRITE_BATCH_SIZE*MEM_WRITE_ROWS_OUT*MEM_WRITE_COLS_OUT*DIVIDE(MEM_WRITE_CHANNELS_OUT,MEM_WRITE_STREAMS_OUT)*MEM_WRITE_WEIGHTS_RELOADING_FACTOR]
)
{
    #pragma HLS INTERFACE axis port=out

    const unsigned size_out = MEM_WRITE_BATCH_SIZE*MEM_WRITE_ROWS_OUT*MEM_WRITE_COLS_OUT*DIVIDE(MEM_WRITE_CHANNELS_OUT,MEM_WRITE_STREAMS_OUT)*MEM_WRITE_WEIGHTS_RELOADING_FACTOR;
    DO_PRAGMA( HLS INTERFACE m_axi port=out_hw depth=size_out num_write_outstanding=1 max_write_burst_length=256)
    #pragma HLS ARRAY_PARTITION variable=out_hw complete dim=1

    #pragma HLS DATAFLOW
    mem_write<
        MEM_WRITE_BATCH_SIZE,
        MEM_WRITE_ROWS_OUT,
        MEM_WRITE_COLS_OUT,
        MEM_WRITE_CHANNELS_OUT,
        MEM_WRITE_PORTS_OUT,
        MEM_WRITE_STREAMS_OUT,
        MEM_WRITE_WEIGHTS_RELOADING_FACTOR,
        data_t
    >(weights_reloading_index, out, out_hw);
}

