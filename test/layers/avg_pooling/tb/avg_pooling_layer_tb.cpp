#include "common_tb.hpp"
#include "avg_pooling_layer_tb.hpp"

int main() {
    int err = 0;

    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(avg_pooling_layer_data_t) in[AVG_POOLING_LAYER_COARSE];
    stream_t(avg_pooling_layer_data_t) out[AVG_POOLING_LAYER_COARSE];
    stream_t(avg_pooling_layer_data_t) out_correct[AVG_POOLING_LAYER_COARSE];

    // test images
    static avg_pooling_layer_data_t test_in[DIVIDE(AVG_POOLING_LAYER_CHANNELS,AVG_POOLING_LAYER_COARSE)*AVG_POOLING_LAYER_ROWS*AVG_POOLING_LAYER_COLS][AVG_POOLING_LAYER_COARSE];
    static avg_pooling_layer_data_t test_out[DIVIDE(AVG_POOLING_LAYER_CHANNELS,AVG_POOLING_LAYER_COARSE)*AVG_POOLING_LAYER_ROWS_OUT*AVG_POOLING_LAYER_COLS_OUT][AVG_POOLING_LAYER_COARSE];

    // load input
    load_data<
        DIVIDE(AVG_POOLING_LAYER_CHANNELS,AVG_POOLING_LAYER_COARSE)*AVG_POOLING_LAYER_ROWS*AVG_POOLING_LAYER_COLS,
        AVG_POOLING_LAYER_COARSE,
        avg_pooling_layer_data_t
    >(input_path,test_in);

    // load output
    load_data<
        DIVIDE(AVG_POOLING_LAYER_CHANNELS,AVG_POOLING_LAYER_COARSE)*AVG_POOLING_LAYER_ROWS_OUT*AVG_POOLING_LAYER_COLS_OUT,
        AVG_POOLING_LAYER_COARSE,
        avg_pooling_layer_data_t
    >(output_path,test_out);

    // convert to streams
    to_stream<
        DIVIDE(AVG_POOLING_LAYER_CHANNELS,AVG_POOLING_LAYER_COARSE)*AVG_POOLING_LAYER_ROWS*AVG_POOLING_LAYER_COLS,
        AVG_POOLING_LAYER_COARSE,
        avg_pooling_layer_data_t
    >(test_in,in);

    to_stream<
        DIVIDE(AVG_POOLING_LAYER_CHANNELS,AVG_POOLING_LAYER_COARSE)*AVG_POOLING_LAYER_ROWS_OUT*AVG_POOLING_LAYER_COLS_OUT,
        AVG_POOLING_LAYER_COARSE,
        avg_pooling_layer_data_t
    >(test_out,out_correct);

    avg_pooling_layer_top(in,out,0);

    for(int i=0;i<AVG_POOLING_LAYER_COARSE;i++) {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<avg_pooling_layer_data_t>(out[i],out_correct[i],false);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err; 
}