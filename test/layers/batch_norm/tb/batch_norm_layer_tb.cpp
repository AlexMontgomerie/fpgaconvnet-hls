#include "common_tb.hpp"
#include "batch_norm_layer_tb.hpp"

int main()
{
    int err = 0;
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(data_t) in[BATCH_NORM_LAYER_COARSE];
    stream_t(data_t) out[BATCH_NORM_LAYER_COARSE];
    stream_t(data_t) out_correct[BATCH_NORM_LAYER_COARSE];

    // test images
    static data_t test_in[CHANNELS_3D(BATCH_NORM_LAYER_CHANNELS,BATCH_NORM_LAYER_COARSE)*BATCH_NORM_LAYER_ROWS*BATCH_NORM_LAYER_COLS][BATCH_NORM_LAYER_COARSE];
    static data_t test_out[CHANNELS_3D(BATCH_NORM_LAYER_CHANNELS,BATCH_NORM_LAYER_COARSE)*BATCH_NORM_LAYER_ROWS*BATCH_NORM_LAYER_COLS][BATCH_NORM_LAYER_COARSE];

    // load input
    load_data<
        CHANNELS_3D(BATCH_NORM_LAYER_CHANNELS,BATCH_NORM_LAYER_COARSE)*BATCH_NORM_LAYER_ROWS*BATCH_NORM_LAYER_COLS,
        BATCH_NORM_LAYER_COARSE
    >(data_path,test_in);

    // load output
    load_data<
        CHANNELS_3D(BATCH_NORM_LAYER_CHANNELS,BATCH_NORM_LAYER_COARSE)*BATCH_NORM_LAYER_ROWS*BATCH_NORM_LAYER_COLS,
        BATCH_NORM_LAYER_COARSE
    >(data_path,test_out);

    // convert to streams
    to_stream<
        CHANNELS_3D(BATCH_NORM_LAYER_CHANNELS,BATCH_NORM_LAYER_COARSE)*BATCH_NORM_LAYER_ROWS*BATCH_NORM_LAYER_COLS,
        BATCH_NORM_LAYER_COARSE
    >(test_in,in);

    // convert to streams
    to_stream<
        CHANNELS_3D(BATCH_NORM_LAYER_CHANNELS,BATCH_NORM_LAYER_COARSE)*BATCH_NORM_LAYER_ROWS*BATCH_NORM_LAYER_COLS,
        BATCH_NORM_LAYER_COARSE
    >(test_out,out_correct);

    batch_norm_layer_top(in,out,0);

    for(int i=0;i<BATCH_NORM_LAYER_COARSE;i++)
    {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<data_t>(out[i],out_correct[i]);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err;
}
