#include "global_pooling_layer_tb.hpp"
#include "global_pooling_layer.hpp"

void global_pooling_layer_top(
  stream_t(global_pooling_layer_data_t) in[GLOBAL_POOLING_LAYER_COARSE],
  stream_t(global_pooling_layer_data_t) out[GLOBAL_POOLING_LAYER_COARSE],
  int mode
)
{
#pragma HLS DATAFLOW
#pragma HLS INTERFACE ap_ctrl_chain port=return

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    global_pooling_layer(in,out,mode);

}