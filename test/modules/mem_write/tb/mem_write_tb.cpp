#include "common_tb.hpp"
#include "mem_write_tb.hpp"

int main()
{

    int err = 0;
    std::string data_path = std::string(DATA_DIR)+"/data.yaml";

    // in/out streams
    stream_t(data_t) in[MEM_WRITE_STREAMS_OUT];

    const int size = MEM_WRITE_BATCH_SIZE*MEM_WRITE_ROWS_OUT*MEM_WRITE_COLS_OUT*MEM_WRITE_CHANNELS_OUT*MEM_WRITE_WEIGHTS_RELOADING_FACTOR;

    // test inputs data
    static data_t test_in[DIVIDE(size,MEM_WRITE_STREAMS_OUT*MEM_WRITE_WEIGHTS_RELOADING_FACTOR)][MEM_WRITE_STREAMS_OUT];
    static mem_int test_out[MEM_WRITE_PORTS_OUT][DIVIDE(size,MEM_WRITE_STREAMS_OUT)] = {0};
    static mem_int test_out_valid[MEM_WRITE_PORTS_OUT][DIVIDE(size,MEM_WRITE_STREAMS_OUT)] = {0};

    // load data_in
    load_wr_data<
        MEM_WRITE_BATCH_SIZE,
        MEM_WRITE_ROWS_OUT,
        MEM_WRITE_COLS_OUT,
        MEM_WRITE_CHANNELS_OUT,
        MEM_WRITE_STREAMS_OUT,
        MEM_WRITE_WEIGHTS_RELOADING_FACTOR,
        MEM_WRITE_WEIGHTS_RELOADING_INDEX
    >(data_path,"data_in",test_in);

    // load data_out
    load_net_data<
        MEM_WRITE_PORTS_OUT,
        MEM_WRITE_BATCH_SIZE,
        MEM_WRITE_ROWS_OUT,
        MEM_WRITE_COLS_OUT,
        MEM_WRITE_CHANNELS_OUT,
        MEM_WRITE_STREAMS_OUT,
        MEM_WRITE_WEIGHTS_RELOADING_FACTOR,
        MEM_WRITE_WEIGHTS_RELOADING_INDEX
    >(data_path,"data_out",test_out_valid);

    // convert input stream
    to_stream<
        DIVIDE(size,MEM_WRITE_STREAMS_OUT*MEM_WRITE_WEIGHTS_RELOADING_FACTOR),
        MEM_WRITE_STREAMS_OUT
    >(test_in,in);

    // run fork
    mem_write_top(MEM_WRITE_WEIGHTS_RELOADING_INDEX,in,test_out);

    for (int i=0;i<MEM_WRITE_PORTS_OUT;i++) {
        err += check_array_equal<DIVIDE(size,MEM_WRITE_STREAMS_OUT)>(test_out[i],test_out_valid[i]);
    }

    return err;
}
