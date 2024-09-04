#include "common_tb.hpp"
#include "global_pool_tb.hpp"

int main()
{
    int err = 0;

    // file paths
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    // in/out streams
    stream_t(global_pool_t) in;
    stream_t(global_pool_t) out;
    stream_t(global_pool_t) out_valid;

    static global_pool_t test_in[GLOBAL_POOL_ROWS*GLOBAL_POOL_COLS*GLOBAL_POOL_CHANNELS];
    static global_pool_t test_out[GLOBAL_POOL_CHANNELS];

    // load data_in
    load_data<
        GLOBAL_POOL_ROWS*GLOBAL_POOL_COLS*GLOBAL_POOL_CHANNELS,
        global_pool_t
    >(input_path,test_in);

    // load data_out
    load_data<
        GLOBAL_POOL_CHANNELS,
        global_pool_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        GLOBAL_POOL_ROWS*GLOBAL_POOL_COLS*GLOBAL_POOL_CHANNELS,
        global_pool_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        GLOBAL_POOL_CHANNELS,
        global_pool_t
    >(test_out,out_valid);

    // run global_pool
    global_pool_top(in,out);

    // check the stream is correct
    err += checkStreamEqual<global_pool_t>(out,out_valid,false);

    return err;
}