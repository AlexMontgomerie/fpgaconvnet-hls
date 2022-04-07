#include "common_tb.hpp"
#include "squeeze_layer_tb.hpp"

int main()
{
    int err = 0;
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(squeeze_layer_data_t) in[SQUEEZE_LAYER_COARSE_IN];
    stream_t(squeeze_layer_data_t) out[SQUEEZE_LAYER_COARSE_OUT];
    stream_t(squeeze_layer_data_t) out_correct[SQUEEZE_LAYER_COARSE_OUT];

    // test images
    static squeeze_layer_data_t test_in[DIVIDE(SQUEEZE_LAYER_CHANNELS,SQUEEZE_LAYER_COARSE_IN)*SQUEEZE_LAYER_ROWS*SQUEEZE_LAYER_COLS][SQUEEZE_LAYER_COARSE_IN];
    static squeeze_layer_data_t test_out[DIVIDE(SQUEEZE_LAYER_CHANNELS,SQUEEZE_LAYER_COARSE_OUT)*SQUEEZE_LAYER_ROWS_OUT*SQUEEZE_LAYER_COLS_OUT][SQUEEZE_LAYER_COARSE_OUT];

    // load input
    load_data<
        DIVIDE(SQUEEZE_LAYER_CHANNELS,SQUEEZE_LAYER_COARSE_IN)*SQUEEZE_LAYER_ROWS*SQUEEZE_LAYER_COLS,
        SQUEEZE_LAYER_COARSE_IN,
        squeeze_layer_data_t
    >(input_path,test_in);

    // load output
    load_data<
        DIVIDE(SQUEEZE_LAYER_CHANNELS,SQUEEZE_LAYER_COARSE_OUT)*SQUEEZE_LAYER_ROWS_OUT*SQUEEZE_LAYER_COLS_OUT,
        SQUEEZE_LAYER_COARSE_OUT,
        squeeze_layer_data_t
    >(output_path,test_out);

    // convert to streams
    to_stream<
        DIVIDE(SQUEEZE_LAYER_CHANNELS,SQUEEZE_LAYER_COARSE_IN)*SQUEEZE_LAYER_ROWS*SQUEEZE_LAYER_COLS,
        SQUEEZE_LAYER_COARSE_IN,
        squeeze_layer_data_t
    >(test_in,in);

    to_stream<
        DIVIDE(SQUEEZE_LAYER_CHANNELS,SQUEEZE_LAYER_COARSE_OUT)*SQUEEZE_LAYER_ROWS_OUT*SQUEEZE_LAYER_COLS_OUT,
        SQUEEZE_LAYER_COARSE_OUT,
        squeeze_layer_data_t
    >(test_out,out_correct);

    squeeze_layer_top(in,out,0);

    for(int i=0;i<SQUEEZE_LAYER_COARSE_OUT;i++)
    {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<squeeze_layer_data_t>(out[i],out_correct[i]);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err;
}
