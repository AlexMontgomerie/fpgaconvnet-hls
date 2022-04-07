#include "common_tb.hpp"
#include "pooling_layer_tb.hpp"

int main()
{
    int err = 0;

    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(pooling_layer_data_t) in[POOLING_LAYER_COARSE];
    stream_t(pooling_layer_data_t) out[POOLING_LAYER_COARSE];
    stream_t(pooling_layer_data_t) out_correct[POOLING_LAYER_COARSE];

    // test images
    static pooling_layer_data_t test_in[DIVIDE(POOLING_LAYER_CHANNELS,POOLING_LAYER_COARSE)*POOLING_LAYER_ROWS*POOLING_LAYER_COLS][POOLING_LAYER_COARSE];
    static pooling_layer_data_t test_out[DIVIDE(POOLING_LAYER_CHANNELS,POOLING_LAYER_COARSE)*POOLING_LAYER_ROWS_OUT*POOLING_LAYER_COLS_OUT][POOLING_LAYER_COARSE];

    // load input
    load_data<
        DIVIDE(POOLING_LAYER_CHANNELS,POOLING_LAYER_COARSE)*POOLING_LAYER_ROWS*POOLING_LAYER_COLS,
        POOLING_LAYER_COARSE,
        pooling_layer_data_t
    >(input_path,test_in);

    // load output
    load_data<
        DIVIDE(POOLING_LAYER_CHANNELS,POOLING_LAYER_COARSE)*POOLING_LAYER_ROWS_OUT*POOLING_LAYER_COLS_OUT,
        POOLING_LAYER_COARSE,
        pooling_layer_data_t
    >(output_path,test_out);

    // convert to streams
    to_stream<
        DIVIDE(POOLING_LAYER_CHANNELS,POOLING_LAYER_COARSE)*POOLING_LAYER_ROWS*POOLING_LAYER_COLS,
        POOLING_LAYER_COARSE,
        pooling_layer_data_t
    >(test_in,in);

    to_stream<
        DIVIDE(POOLING_LAYER_CHANNELS,POOLING_LAYER_COARSE)*POOLING_LAYER_ROWS_OUT*POOLING_LAYER_COLS_OUT,
        POOLING_LAYER_COARSE,
        pooling_layer_data_t
    >(test_out,out_correct);

    pooling_layer_top(in,out,0);

    for(int i=0;i<POOLING_LAYER_COARSE;i++)
    {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<pooling_layer_data_t>(out[i],out_correct[i],false);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err;
}
