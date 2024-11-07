#include "common_tb.hpp"
#include "avg_pool_tb.hpp"

int main()
{
    int err = 0;

    // file paths
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    // in/out streams
    stream_t(avg_pool_t) in[AVG_POOL_KERNEL_SIZE_0][AVG_POOL_KERNEL_SIZE_1];
    stream_t(avg_pool_t) out;
    stream_t(avg_pool_t) out_valid;

    static avg_pool_t test_in[AVG_POOL_ROWS*AVG_POOL_COLS*AVG_POOL_CHANNELS][AVG_POOL_KERNEL_SIZE_0][AVG_POOL_KERNEL_SIZE_1];
    static avg_pool_t test_out[AVG_POOL_ROWS*AVG_POOL_COLS*AVG_POOL_CHANNELS];

    // load data_in
    load_data<
        AVG_POOL_ROWS*AVG_POOL_COLS*AVG_POOL_CHANNELS,
        AVG_POOL_KERNEL_SIZE_0,
        AVG_POOL_KERNEL_SIZE_1,
        avg_pool_t
    >(input_path,test_in);

    // load data_out
    load_data<
        AVG_POOL_ROWS*AVG_POOL_COLS*AVG_POOL_CHANNELS,
        avg_pool_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        AVG_POOL_ROWS*AVG_POOL_COLS*AVG_POOL_CHANNELS,
        AVG_POOL_KERNEL_SIZE_0,
        AVG_POOL_KERNEL_SIZE_1,
        avg_pool_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        AVG_POOL_ROWS*AVG_POOL_COLS*AVG_POOL_CHANNELS,
        avg_pool_t
    >(test_out,out_valid);

    // run avg_pool
    avg_pool_top(in,out);

    // check the stream is correct
    err += checkStreamEqual<avg_pool_t>(out,out_valid,false);

    return err;

}