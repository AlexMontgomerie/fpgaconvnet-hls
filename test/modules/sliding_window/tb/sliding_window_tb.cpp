#include "common_tb.hpp"
#include "sliding_window_tb.hpp"

int main()
{
    int err = 0;

    // file paths
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(sliding_window_t) in;
    stream_t(sliding_window_t) out[SLIDING_WINDOW_KERNEL_SIZE_0][SLIDING_WINDOW_KERNEL_SIZE_1];
    stream_t(sliding_window_t) out_valid[SLIDING_WINDOW_KERNEL_SIZE_0][SLIDING_WINDOW_KERNEL_SIZE_1];

    // test images
    static sliding_window_t test_in[SLIDING_WINDOW_BATCH_SIZE*SLIDING_WINDOW_CHANNELS*SLIDING_WINDOW_ROWS*SLIDING_WINDOW_COLS];
    static sliding_window_t test_out[SLIDING_WINDOW_BATCH_SIZE*SLIDING_WINDOW_CHANNELS*SLIDING_WINDOW_ROWS_OUT*SLIDING_WINDOW_COLS_OUT][SLIDING_WINDOW_KERNEL_SIZE_0][SLIDING_WINDOW_KERNEL_SIZE_1];

    // load input
    load_data<
        SLIDING_WINDOW_BATCH_SIZE*SLIDING_WINDOW_CHANNELS*SLIDING_WINDOW_ROWS*SLIDING_WINDOW_COLS,
        sliding_window_t
    >(input_path,test_in);

    // load output
    load_data<
        SLIDING_WINDOW_BATCH_SIZE*SLIDING_WINDOW_CHANNELS*SLIDING_WINDOW_ROWS_OUT*SLIDING_WINDOW_COLS_OUT,
        SLIDING_WINDOW_KERNEL_SIZE_0,
        SLIDING_WINDOW_KERNEL_SIZE_1,
        sliding_window_t
    >(output_path,test_out);

    to_stream<
        SLIDING_WINDOW_BATCH_SIZE*SLIDING_WINDOW_CHANNELS*SLIDING_WINDOW_ROWS*SLIDING_WINDOW_COLS,
        sliding_window_t
    >(test_in,in);

    to_stream<
        SLIDING_WINDOW_BATCH_SIZE*SLIDING_WINDOW_CHANNELS*SLIDING_WINDOW_ROWS_OUT*SLIDING_WINDOW_COLS_OUT,
        SLIDING_WINDOW_KERNEL_SIZE_0,
        SLIDING_WINDOW_KERNEL_SIZE_1,
        sliding_window_t
    >(test_out,out_valid);

    // run sliding window
    sliding_window_top(in,out);

    for(int k1=0;k1<SLIDING_WINDOW_KERNEL_SIZE_0;k1++) {
        for(int k2=0;k2<SLIDING_WINDOW_KERNEL_SIZE_1;k2++) {
            err += checkStreamEqual<sliding_window_t>(out[k1][k2],out_valid[k1][k2]);
        }
    }
    return err;
}
