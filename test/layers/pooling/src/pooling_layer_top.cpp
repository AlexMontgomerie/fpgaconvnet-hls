#include "pooling_layer_tb.hpp"
#include "pooling_layer.hpp"

void pooling_layer_top(
  stream_t(pooling_layer_data_t) in[POOLING_LAYER_COARSE],
  stream_t(pooling_layer_data_t) out[POOLING_LAYER_COARSE],
  int mode
)
{
#pragma HLS DATAFLOW
#pragma HLS INTERFACE ap_ctrl_chain port=return

//#pragma HLS INTERFACE axis port=in
//#pragma HLS INTERFACE axis port=out
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    pooling_layer(in,out,mode);

}
