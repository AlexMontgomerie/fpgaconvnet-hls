#include "common_tb.hpp"
#include "mem_write_tb.hpp"

int main()
{

    int err = 0;
    std::string data_path  = std::string(DATA_DIR)+"/data.dat";

    const unsigned size_out = MEM_WRITE_BATCH_SIZE*MEM_WRITE_ROWS_OUT*MEM_WRITE_COLS_OUT*DIVIDE(MEM_WRITE_CHANNELS_OUT,MEM_WRITE_STREAMS_OUT)*MEM_WRITE_WEIGHTS_RELOADING_FACTOR;

    // test inputs data
    static data_t test_in[DIVIDE(size_out,MEM_WRITE_WEIGHTS_RELOADING_FACTOR)][MEM_WRITE_STREAMS_OUT];
    stream_t(data_t) in[MEM_WRITE_STREAMS_OUT];

    static mem_int test_out[MEM_WRITE_PORTS_OUT][size_out] = {0};
    static mem_int test_out_valid[MEM_WRITE_PORTS_OUT][size_out] = {0};

    // get number of dma channels
    const unsigned dma_channels = DIVIDE(MEM_WRITE_DMA_WIDTH,MEM_WRITE_DATA_WIDTH);

    for(int wr_index=0; wr_index<MEM_WRITE_WEIGHTS_RELOADING_FACTOR; wr_index++) {

        // load test in data
        load_wr_data<
            MEM_WRITE_BATCH_SIZE,
            MEM_WRITE_ROWS_OUT,
            MEM_WRITE_COLS_OUT,
            MEM_WRITE_CHANNELS_OUT,
            MEM_WRITE_STREAMS_OUT,
            MEM_WRITE_WEIGHTS_RELOADING_FACTOR,
            data_t
        >(data_path, test_in, wr_index);

        // convert to a stream
        to_stream<
            DIVIDE(size_out, MEM_WRITE_WEIGHTS_RELOADING_FACTOR),
            MEM_WRITE_STREAMS_OUT
        >(test_in, in);

        // add to valid test out
        const unsigned channels_per_stream = DIVIDE(MEM_WRITE_CHANNELS_OUT, MEM_WRITE_STREAMS_OUT);
        for(int i=0;i<MEM_WRITE_BATCH_SIZE*MEM_WRITE_ROWS_OUT*MEM_WRITE_COLS_OUT; i++) {
            for(int j=0; j<channels_per_stream; j++) {
                for(int k=0; k<MEM_WRITE_STREAMS_OUT; k++) {
                    test_out_valid[0][i*channels_per_stream*MEM_WRITE_WEIGHTS_RELOADING_FACTOR+wr_index*channels_per_stream+j] |=
                        ( ( test_in[i*channels_per_stream+j][k].range() & MEM_WRITE_BIT_MASK ) <<
                          ( ( k % dma_channels ) * MEM_WRITE_DATA_WIDTH ) );
                }
            }
        }

        // run mem write for wr index
        mem_write_top(wr_index, in, test_out);

    }

    for (int i=0;i<MEM_WRITE_PORTS_OUT; i++) {
        err += check_array_equal<size_out, MEM_WRITE_STREAMS_OUT>(test_out[i],test_out_valid[i]);
    }

    return err;
}
