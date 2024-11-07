#include "common_tb.hpp"
#include "global_pooling_layer_tb.hpp"

int main() {
    int err = 0; 

    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(global_pooling_layer_data_t) in[GLOBAL_POOLING_LAYER_COARSE];
    stream_t(global_pooling_layer_data_t) out[GLOBAL_POOLING_LAYER_COARSE];
    stream_t(global_pooling_layer_data_t) out_correct[GLOBAL_POOLING_LAYER_COARSE];

    // test images
    static global_pooling_layer_data_t test_in[DIVIDE(GLOBAL_POOLING_LAYER_CHANNELS,GLOBAL_POOLING_LAYER_COARSE)*GLOBAL_POOLING_LAYER_ROWS*GLOBAL_POOLING_LAYER_COLS][GLOBAL_POOLING_LAYER_COARSE];
    static global_pooling_layer_data_t test_out[DIVIDE(GLOBAL_POOLING_LAYER_CHANNELS,GLOBAL_POOLING_LAYER_COARSE)*GLOBAL_POOLING_LAYER_ROWS_OUT*GLOBAL_POOLING_LAYER_COLS_OUT][GLOBAL_POOLING_LAYER_COARSE];

    // load input
    load_data<
        DIVIDE(GLOBAL_POOLING_LAYER_CHANNELS,GLOBAL_POOLING_LAYER_COARSE)*GLOBAL_POOLING_LAYER_ROWS*GLOBAL_POOLING_LAYER_COLS,
        GLOBAL_POOLING_LAYER_COARSE,
        global_pooling_layer_data_t
    >(input_path,test_in);

    // load output
    load_data<
        DIVIDE(GLOBAL_POOLING_LAYER_CHANNELS,GLOBAL_POOLING_LAYER_COARSE)*GLOBAL_POOLING_LAYER_ROWS_OUT*GLOBAL_POOLING_LAYER_COLS_OUT,
        GLOBAL_POOLING_LAYER_COARSE,
        global_pooling_layer_data_t
    >(output_path,test_out);

    // convert to streams
    to_stream<
        DIVIDE(GLOBAL_POOLING_LAYER_CHANNELS,GLOBAL_POOLING_LAYER_COARSE)*GLOBAL_POOLING_LAYER_ROWS*GLOBAL_POOLING_LAYER_COLS,
        GLOBAL_POOLING_LAYER_COARSE,
        global_pooling_layer_data_t
    >(test_in,in);

    to_stream<
        DIVIDE(GLOBAL_POOLING_LAYER_CHANNELS,GLOBAL_POOLING_LAYER_COARSE)*GLOBAL_POOLING_LAYER_ROWS_OUT*GLOBAL_POOLING_LAYER_COLS_OUT,
        GLOBAL_POOLING_LAYER_COARSE,
        global_pooling_layer_data_t
    >(test_out,out_correct);

    global_pooling_layer_top(in,out,0);

    for(int i=0;i<GLOBAL_POOLING_LAYER_COARSE;i++) {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<global_pooling_layer_data_t>(out[i],out_correct[i],false);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err;
}