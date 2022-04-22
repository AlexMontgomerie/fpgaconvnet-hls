#include "common_tb.hpp"
#include "mem_read_tb.hpp"

int main()
{

    int err = 0;
    std::string data_path  = std::string(DATA_DIR)+"/data.dat";

    const int size_in = MEM_READ_BATCH_SIZE*MEM_READ_ROWS_IN*MEM_READ_COLS_IN*DIVIDE(MEM_READ_CHANNELS_IN,MEM_READ_STREAMS_IN);

    // in/out streams
    stream_t(data_t) out[MEM_READ_STREAMS_IN];
    stream_t(data_t) out_valid[MEM_READ_STREAMS_IN];

    // test inputs data
    static mem_int test_in[MEM_READ_PORTS_IN][size_in] = {0};
    static data_t test_stream[size_in][MEM_READ_STREAMS_IN];

    // load test stream
    load_data<
        size_in,
        MEM_READ_STREAMS_IN,
        data_t
    >(data_path,test_stream);

    // get number of dma channels
    int dma_channels = DIVIDE(MEM_READ_DMA_WIDTH,MEM_READ_DATA_WIDTH);

    // convert test stream to mem_int
    for(int i=0;i<size_in;i++) {
        for(int j=0;j<MEM_READ_STREAMS_IN;j++) {
            test_in[(int)(j/dma_channels)][i] |= ( ( test_stream[i][j].range() & MEM_READ_BIT_MASK ) <<
                    ( ( j % dma_channels ) * MEM_READ_DATA_WIDTH ) );
        }
    }

    // convert to out valid stream
    to_stream<
        size_in,
        MEM_READ_STREAMS_IN,
        data_t
    >(test_stream,out_valid);

    // run mem read
    mem_read_top(test_in,out);

    // check for errors
    for(int i=0;i<MEM_READ_STREAMS_IN;i++) {
        err += checkStreamEqual<data_t>(out[i],out_valid[i],false);
    }

    return err;
}
