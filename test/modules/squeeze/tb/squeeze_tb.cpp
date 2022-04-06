#include "common_tb.hpp"
#include "squeeze_tb.hpp"

int main()
{

    int err = 0;
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    // in/out streams
    stream_t(squeeze_t) in[SQUEEZE_COARSE_IN];
    stream_t(squeeze_t) out[SQUEEZE_COARSE_OUT];
    stream_t(squeeze_t) out_valid[SQUEEZE_COARSE_OUT];

    // test inputs data
    static squeeze_t test_in[SQUEEZE_BATCH_SIZE*SQUEEZE_ROWS*SQUEEZE_COLS*DIVIDE(SQUEEZE_CHANNELS,SQUEEZE_COARSE_IN)][SQUEEZE_COARSE_IN];
    static squeeze_t test_out[SQUEEZE_BATCH_SIZE*SQUEEZE_ROWS*SQUEEZE_COLS*DIVIDE(SQUEEZE_CHANNELS,SQUEEZE_COARSE_OUT)][SQUEEZE_COARSE_OUT];

    // load data_in
    load_data<
        SQUEEZE_BATCH_SIZE*SQUEEZE_ROWS*SQUEEZE_COLS*DIVIDE(SQUEEZE_CHANNELS,SQUEEZE_COARSE_IN),
        SQUEEZE_COARSE_IN,
        squeeze_t
    >(input_path,test_in);

    // load data_out
    load_data<
        SQUEEZE_BATCH_SIZE*SQUEEZE_ROWS*SQUEEZE_COLS*DIVIDE(SQUEEZE_CHANNELS,SQUEEZE_COARSE_OUT),
        SQUEEZE_COARSE_OUT,
        squeeze_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        SQUEEZE_BATCH_SIZE*SQUEEZE_ROWS*SQUEEZE_COLS*DIVIDE(SQUEEZE_CHANNELS,SQUEEZE_COARSE_IN),
        SQUEEZE_COARSE_IN,
        squeeze_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        SQUEEZE_BATCH_SIZE*SQUEEZE_ROWS*SQUEEZE_COLS*DIVIDE(SQUEEZE_CHANNELS,SQUEEZE_COARSE_OUT),
        SQUEEZE_COARSE_OUT,
        squeeze_t
    >(test_out,out_valid);

    // run squeeze
    squeeze_top(in,out);

    for(int j=0;j<SQUEEZE_COARSE_OUT;j++) {
        err += checkStreamEqual<squeeze_t>(out[j],out_valid[j], false);
    }

    return err;
}
