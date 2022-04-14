#include "common_tb.hpp"
#include "inner_product_layer_tb.hpp"

int main()
{
    int err = 0;
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    const int rows_out = 1;
    const int cols_out = 1;

    stream_t(inner_product_layer_input_t) in[INNER_PRODUCT_LAYER_COARSE_IN];
    stream_t(inner_product_layer_output_t) out[INNER_PRODUCT_LAYER_COARSE_OUT];
    stream_t(inner_product_layer_output_t) out_correct[INNER_PRODUCT_LAYER_COARSE_OUT];

    // test images
    static inner_product_layer_input_t test_in[DIVIDE(INNER_PRODUCT_LAYER_CHANNELS,INNER_PRODUCT_LAYER_COARSE_IN)*INNER_PRODUCT_LAYER_ROWS*INNER_PRODUCT_LAYER_COLS][INNER_PRODUCT_LAYER_COARSE_IN];
    static inner_product_layer_output_t test_out[DIVIDE(INNER_PRODUCT_LAYER_FILTERS,INNER_PRODUCT_LAYER_COARSE_OUT)*rows_out*cols_out][INNER_PRODUCT_LAYER_COARSE_OUT];

    // load input
    load_data<
        DIVIDE(INNER_PRODUCT_LAYER_CHANNELS,INNER_PRODUCT_LAYER_COARSE_IN)*INNER_PRODUCT_LAYER_ROWS*INNER_PRODUCT_LAYER_COLS,
        INNER_PRODUCT_LAYER_COARSE_IN,
        inner_product_layer_input_t
    >(input_path,test_in);

    // load output
    load_data<
        DIVIDE(INNER_PRODUCT_LAYER_FILTERS,INNER_PRODUCT_LAYER_COARSE_OUT)*rows_out*cols_out,
        INNER_PRODUCT_LAYER_COARSE_OUT,
        inner_product_layer_output_t
    >(output_path,test_out);

    // convert to streams
    to_stream<
        DIVIDE(INNER_PRODUCT_LAYER_CHANNELS,INNER_PRODUCT_LAYER_COARSE_IN)*INNER_PRODUCT_LAYER_ROWS*INNER_PRODUCT_LAYER_COLS,
        INNER_PRODUCT_LAYER_COARSE_IN,
        inner_product_layer_input_t
    >(test_in,in);

    to_stream<
        DIVIDE(INNER_PRODUCT_LAYER_FILTERS,INNER_PRODUCT_LAYER_COARSE_OUT)*rows_out*cols_out,
        INNER_PRODUCT_LAYER_COARSE_OUT,
        inner_product_layer_output_t
    >(test_out,out_correct);

    inner_product_layer_top(in,out,0);

    for(int i=0;i<INNER_PRODUCT_LAYER_COARSE_OUT;i++)
    {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<inner_product_layer_output_t>(out[i],out_correct[i]);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err;
}
