#include "common_tb.hpp"
#include "elementwise_mul_layer_tb.hpp"

int main()
{
    int err = 0; 
    std::string input_1_path = std::string(DATA_DIR)+"/input_1.dat";
    std::string input_2_path = std::string(DATA_DIR)+"/input_2.dat";
    std::string output_path  = std::string(DATA_DIR)+"/output.dat";

    stream_t(elementwise_mul_layer_data_t) in1[ELEMENTWISE_MUL_LAYER_COARSE];
    stream_t(elementwise_mul_layer_data_t) in2[ELEMENTWISE_MUL_LAYER_COARSE];
    stream_t(elementwise_mul_layer_data_t) out[ELEMENTWISE_MUL_LAYER_COARSE];
    stream_t(elementwise_mul_layer_data_t) out_correct[ELEMENTWISE_MUL_LAYER_COARSE];

    // test images
    static elementwise_mul_layer_data_t test_in1[DIVIDE(ELEMENTWISE_MUL_LAYER_CHANNELS,ELEMENTWISE_MUL_LAYER_COARSE)*ELEMENTWISE_MUL_LAYER_ROWS*ELEMENTWISE_MUL_LAYER_COLS][ELEMENTWISE_MUL_LAYER_COARSE];
    static elementwise_mul_layer_data_t test_in2[DIVIDE(ELEMENTWISE_MUL_LAYER_CHANNELS,ELEMENTWISE_MUL_LAYER_COARSE)*ELEMENTWISE_MUL_LAYER_ROWS*ELEMENTWISE_MUL_LAYER_COLS][ELEMENTWISE_MUL_LAYER_COARSE];
    static elementwise_mul_layer_data_t test_out[DIVIDE(ELEMENTWISE_MUL_LAYER_CHANNELS,ELEMENTWISE_MUL_LAYER_COARSE)*ELEMENTWISE_MUL_LAYER_ROWS*ELEMENTWISE_MUL_LAYER_COLS][ELEMENTWISE_MUL_LAYER_COARSE];

    // load input 
    load_data<
        DIVIDE(ELEMENTWISE_MUL_LAYER_CHANNELS,ELEMENTWISE_MUL_LAYER_COARSE)*ELEMENTWISE_MUL_LAYER_ROWS*ELEMENTWISE_MUL_LAYER_COLS,
        ELEMENTWISE_MUL_LAYER_COARSE,
        elementwise_mul_layer_data_t
    >(input_1_path,test_in1);

    load_data<
        DIVIDE(ELEMENTWISE_MUL_LAYER_CHANNELS,ELEMENTWISE_MUL_LAYER_COARSE)*ELEMENTWISE_MUL_LAYER_ROWS*ELEMENTWISE_MUL_LAYER_COLS,
        ELEMENTWISE_MUL_LAYER_COARSE,
        elementwise_mul_layer_data_t
    >(input_2_path,test_in2);

    // load output
    load_data<
        DIVIDE(ELEMENTWISE_MUL_LAYER_CHANNELS,ELEMENTWISE_MUL_LAYER_COARSE)*ELEMENTWISE_MUL_LAYER_ROWS*ELEMENTWISE_MUL_LAYER_COLS,
        ELEMENTWISE_MUL_LAYER_COARSE,
        elementwise_mul_layer_data_t
    >(output_path,test_out);

    // convert to streams
    to_stream<
        DIVIDE(ELEMENTWISE_MUL_LAYER_CHANNELS,ELEMENTWISE_MUL_LAYER_COARSE)*ELEMENTWISE_MUL_LAYER_ROWS*ELEMENTWISE_MUL_LAYER_COLS,
        ELEMENTWISE_MUL_LAYER_COARSE,
        elementwise_mul_layer_data_t
    >(test_in1,in1);

    to_stream<
        DIVIDE(ELEMENTWISE_MUL_LAYER_CHANNELS,ELEMENTWISE_MUL_LAYER_COARSE)*ELEMENTWISE_MUL_LAYER_ROWS*ELEMENTWISE_MUL_LAYER_COLS,
        ELEMENTWISE_MUL_LAYER_COARSE,
        elementwise_mul_layer_data_t
    >(test_in2,in2);

    to_stream<
        DIVIDE(ELEMENTWISE_MUL_LAYER_CHANNELS,ELEMENTWISE_MUL_LAYER_COARSE)*ELEMENTWISE_MUL_LAYER_ROWS*ELEMENTWISE_MUL_LAYER_COLS,
        ELEMENTWISE_MUL_LAYER_COARSE,
        elementwise_mul_layer_data_t
    >(test_out,out_correct);

    elementwise_mul_layer_top(in1,in2,out,0);

    for (int i = 0; i < ELEMENTWISE_MUL_LAYER_COARSE; i++)
    {
        printf("TESTING OUTPUT %d: ", i);
        err += checkStreamEqual<elementwise_mul_layer_data_t>(out[i], out_correct[i]);
        printf("%s\n", (err == 0) ? "passed" : "failed");
    }
}