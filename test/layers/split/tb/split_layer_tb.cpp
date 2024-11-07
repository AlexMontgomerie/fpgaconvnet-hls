#include "common_tb.hpp"
#include "split_layer_tb.hpp"

int main()
{
    int err = 0;
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(split_layer_data_t) in[SPLIT_LAYER_COARSE];
    stream_t(split_layer_data_t) out_1[SPLIT_LAYER_COARSE];
    stream_t(split_layer_data_t) out_2[SPLIT_LAYER_COARSE];
    stream_t(split_layer_data_t) out_correct_1[SPLIT_LAYER_COARSE];
    stream_t(split_layer_data_t) out_correct_2[SPLIT_LAYER_COARSE];

    // test images
    static split_layer_data_t test_in[DIVIDE(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS*SPLIT_LAYER_COLS][SPLIT_LAYER_COARSE];
    static split_layer_data_t test_out[DIVIDE(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS_OUT*SPLIT_LAYER_COLS_OUT][SPLIT_LAYER_COARSE];

    // load input
    load_data<
        DIVIDE(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS*SPLIT_LAYER_COLS,
        SPLIT_LAYER_COARSE,
        split_layer_data_t
    >(input_path,test_in);

    // load output
    load_data<
        DIVIDE(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS_OUT*SPLIT_LAYER_COLS_OUT,
        SPLIT_LAYER_COARSE,
        split_layer_data_t
    >(output_path,test_out);

    // convert to streams
    to_stream<
        DIVIDE(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS*SPLIT_LAYER_COLS,
        SPLIT_LAYER_COARSE,
        split_layer_data_t
    >(test_in,in);

    to_stream<
        DIVIDE(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS_OUT*SPLIT_LAYER_COLS_OUT,
        SPLIT_LAYER_COARSE,
        split_layer_data_t
    >(test_out,out_correct_1);

    to_stream<
        DIVIDE(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS_OUT*SPLIT_LAYER_COLS_OUT,
        SPLIT_LAYER_COARSE,
        split_layer_data_t
    >(test_out,out_correct_2);

    split_layer_top(in,out_1,out_2,0);

    for(int i=0;i<SPLIT_LAYER_COARSE;i++)
    {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<split_layer_data_t>(out_1[i],out_correct_1[i]);
        err += checkStreamEqual<split_layer_data_t>(out_2[i],out_correct_2[i]);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err;
}