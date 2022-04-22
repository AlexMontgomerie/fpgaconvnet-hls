#include "common_tb.hpp"
#include "fork_tb.hpp"

int main()
{

    int err = 0;

    // file paths
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

#if (FORK_KERNEL_SIZE_0 > 1) || (FORK_KERNEL_SIZE_1 > 1)
    // in/out streams
    stream_t(fork_t) in[FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1];
    stream_t(fork_t) out[FORK_COARSE][FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1];
    stream_t(fork_t) out_valid[FORK_COARSE][FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1];

    // test inputs data
    static fork_t test_in[FORK_ROWS*FORK_COLS*FORK_CHANNELS][FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1];
    static fork_t test_out[FORK_ROWS*FORK_COLS*FORK_CHANNELS][FORK_COARSE][FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1];
#else
    // in/out streams
    stream_t(fork_t) in;
    stream_t(fork_t) out[FORK_COARSE];
    stream_t(fork_t) out_valid[FORK_COARSE];

    // test inputs data
    static fork_t test_in[FORK_ROWS*FORK_COLS*FORK_CHANNELS];
    static fork_t test_out[FORK_ROWS*FORK_COLS*FORK_CHANNELS][FORK_COARSE];
#endif

    // load data_in
    load_data<
        FORK_ROWS*FORK_COLS*FORK_CHANNELS,
#if (FORK_KERNEL_SIZE_0 > 1) || (FORK_KERNEL_SIZE_1 > 1)
        FORK_KERNEL_SIZE_0,
        FORK_KERNEL_SIZE_1,
#endif
        fork_t
    >(input_path,test_in);

    // load data_out
    load_data<
        FORK_ROWS*FORK_COLS*FORK_CHANNELS,
        FORK_COARSE,
#if (FORK_KERNEL_SIZE_0 > 1) || (FORK_KERNEL_SIZE_1 > 1)
        FORK_KERNEL_SIZE_0,
        FORK_KERNEL_SIZE_1,
#endif
        fork_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        FORK_ROWS*FORK_COLS*FORK_CHANNELS,
#if (FORK_KERNEL_SIZE_0 > 1) || (FORK_KERNEL_SIZE_1 > 1)
        FORK_KERNEL_SIZE_0,
        FORK_KERNEL_SIZE_1,
#endif
        fork_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        FORK_ROWS*FORK_COLS*FORK_CHANNELS,
        FORK_COARSE,
#if (FORK_KERNEL_SIZE_0 > 1) || (FORK_KERNEL_SIZE_1 > 1)
        FORK_KERNEL_SIZE_0,
        FORK_KERNEL_SIZE_1,
#endif
        fork_t
    >(test_out,out_valid);

    // run fork
    fork_top(in,out);

#if (FORK_KERNEL_SIZE_0 > 1) || (FORK_KERNEL_SIZE_1 > 1)
    for(int j=0;j<FORK_COARSE;j++) {
        for(int k1=0;k1<FORK_KERNEL_SIZE_0;k1++) {
	        for(int k2=0;k2<FORK_KERNEL_SIZE_1;k2++) {
	            err += checkStreamEqual<fork_t>(out[j][k1][k2],out_valid[j][k1][k2]);
	        }
	    }
    }
#else
    for(int j=0;j<FORK_COARSE;j++) {
	    err += checkStreamEqual<fork_t>(out[j],out_valid[j]);
    }
#endif

    return err;
}
