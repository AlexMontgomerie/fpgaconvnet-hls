#include "common_tb.hpp"
#include "conv_tb.hpp"

int main()
{
    int err = 0;

    std::string input_path      = std::string(DATA_DIR)+"/input.dat";
    std::string output_path     = std::string(DATA_DIR)+"/output.dat";
    std::string weights_path    = std::string(DATA_DIR)+"/weights.dat";

    // weights
    conv_weight_t weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUPS)][CONV_KERNEL_SIZE_0][CONV_KERNEL_SIZE_1];

    stream_t(conv_data_t) in[CONV_KERNEL_SIZE_0][CONV_KERNEL_SIZE_1];
    stream_t(conv_acc_t) out("out");
    stream_t(conv_acc_t) out_valid("out_valid");

    // test inputs data
    static conv_data_t test_in[CONV_ROWS*CONV_COLS*CONV_CHANNELS][CONV_KERNEL_SIZE_0][CONV_KERNEL_SIZE_1];
    static conv_acc_t test_out[CONV_ROWS*CONV_COLS*CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUPS)];

    // load weights
    load_data<
        CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUPS),
        CONV_KERNEL_SIZE_0,
        CONV_KERNEL_SIZE_1,
        conv_weight_t
    >(weights_path,weights);

    // load data_in
    load_data<
        CONV_ROWS*CONV_COLS*CONV_CHANNELS,
        CONV_KERNEL_SIZE_0,
        CONV_KERNEL_SIZE_1,
        conv_data_t
    >(input_path,test_in);

    // load data_out
    load_data<
        CONV_ROWS*CONV_COLS*CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUPS),
        conv_acc_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        CONV_ROWS*CONV_COLS*CONV_CHANNELS,
        CONV_KERNEL_SIZE_0,
        CONV_KERNEL_SIZE_1,
        conv_data_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        CONV_ROWS*CONV_COLS*CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUPS),
        conv_acc_t
    >(test_out,out_valid);

    // run conv
    conv_top(in,weights,out);

    // check output
    err += checkStreamEqual<conv_acc_t>(out,out_valid);

    return err;

}
