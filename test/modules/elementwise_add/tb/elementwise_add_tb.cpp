#include "common_tb.hpp"
#include "elementwise_add_tb.hpp"

int main()
{
    int err = 0;
    std::string input_path_1  = std::string(DATA_DIR)+"/input_1.dat";
    std::string input_path_2  = std::string(DATA_DIR)+"/input_2.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    // in/out streams
    stream_t(elementwise_add_t) in1;
    stream_t(elementwise_add_t) in2;
    stream_t(elementwise_add_t) out;
    stream_t(elementwise_add_t) out_valid;

    // test inputs data
    static elementwise_add_t test_in1[ELEMENTWISE_ADD_ROWS*ELEMENTWISE_ADD_COLS*ELEMENTWISE_ADD_CHANNELS];
    static elementwise_add_t test_in2[ELEMENTWISE_ADD_ROWS*ELEMENTWISE_ADD_COLS*ELEMENTWISE_ADD_CHANNELS];
    static elementwise_add_t test_out[ELEMENTWISE_ADD_ROWS*ELEMENTWISE_ADD_COLS*ELEMENTWISE_ADD_CHANNELS];

    // load data_in
    load_data<
        ELEMENTWISE_ADD_ROWS*ELEMENTWISE_ADD_COLS*ELEMENTWISE_ADD_CHANNELS,
        elementwise_add_t
    >(input_path_1,test_in1);

    load_data<
        ELEMENTWISE_ADD_ROWS*ELEMENTWISE_ADD_COLS*ELEMENTWISE_ADD_CHANNELS,
        elementwise_add_t
    >(input_path_2,test_in2);

    // load data_out
    load_data<
        ELEMENTWISE_ADD_ROWS*ELEMENTWISE_ADD_COLS*ELEMENTWISE_ADD_CHANNELS,
        elementwise_add_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        ELEMENTWISE_ADD_ROWS*ELEMENTWISE_ADD_COLS*ELEMENTWISE_ADD_CHANNELS,
        elementwise_add_t
    >(test_in1,in1);

    to_stream<
        ELEMENTWISE_ADD_ROWS*ELEMENTWISE_ADD_COLS*ELEMENTWISE_ADD_CHANNELS,
        elementwise_add_t
    >(test_in2,in2);

    // convert to out valid stream
    to_stream<
        ELEMENTWISE_ADD_ROWS*ELEMENTWISE_ADD_COLS*ELEMENTWISE_ADD_CHANNELS,
        elementwise_add_t
    >(test_out,out_valid);

    // run elementwise_add
    elementwise_add_top(in1,in2,out);

    err += checkStreamEqual<elementwise_add_t>(out,out_valid,false);

    return err;

}
