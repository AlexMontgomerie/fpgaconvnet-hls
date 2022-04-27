#include "common_tb.hpp"
#include "bias_tb.hpp"

int main()
{
    int err = 0;

    std::string input_path      = std::string(DATA_DIR)+"/input.dat";
    std::string output_path     = std::string(DATA_DIR)+"/output.dat";
    std::string biases_path    = std::string(DATA_DIR)+"/biases.dat";

    // biases
    //bias_weight_t biases[BIAS_CHANNELS*DIVIDE(BIAS_FILTERS,BIAS_GROUPS)];
    //const unsigned int f_per_cout = DIVIDE(BIAS_FILTERS,BIAS_COARSE_OUT);
    bias_biases_t biases[BIAS_FILTERS];

    stream_t(bias_data_t) in("in");
    stream_t(bias_data_t) out("out");
    stream_t(bias_data_t) out_valid("out_valid");

    // test inputs data
    static bias_data_t test_in[BIAS_ROWS*BIAS_COLS*BIAS_FILTERS];
    static bias_data_t test_out[BIAS_ROWS*BIAS_COLS*BIAS_FILTERS]; //TODO check

    // load biases
    load_data<
        BIAS_FILTERS,
        bias_biases_t
    >(biases_path,biases);

    // load data_in
    load_data<
        BIAS_ROWS*BIAS_COLS*BIAS_FILTERS,
        bias_data_t
    >(input_path,test_in);

    // load data_out
    load_data<
        BIAS_ROWS*BIAS_COLS*BIAS_FILTERS,
        bias_data_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        BIAS_ROWS*BIAS_COLS*BIAS_FILTERS,
        bias_data_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        BIAS_ROWS*BIAS_COLS*BIAS_FILTERS,
        bias_data_t
    >(test_out,out_valid);

    // run bias
    bias_top(in,biases,out);

    // check output
    //for(int j=0;j<BIAS_COARSE_OUT;j++) {
    //    err += checkStreamEqual<bias_data_t>(out[j],out_valid[j]);
    //}
    err += checkStreamEqual<bias_data_t>(out,out_valid);

    return err;
}
