#include "elementwise_mul_layer_tb.hpp"
#include "elementwise_mul_layer.hpp"

void elementwise_mul_layer_top(
  stream_t(elementwise_mul_layer_data_t) in1[ELEMENTWISE_MUL_LAYER_COARSE],
  stream_t(elementwise_mul_layer_data_t) in2[ELEMENTWISE_MUL_LAYER_COARSE],
  stream_t(elementwise_mul_layer_data_t) out[ELEMENTWISE_MUL_LAYER_COARSE],
  int mode
)
{

#pragma HLS DATAFLOW
#pragma HLS INTERFACE ap_ctrl_chain port=return

#pragma HLS STREAM variable=in1
#pragma HLS STREAM variable=in2
#pragma HLS STREAM variable=out

    elementwise_mul_layer(in1,in2,out,mode);

}