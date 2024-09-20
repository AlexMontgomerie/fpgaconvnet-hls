#include "avg_pooling_layer_tb.hpp"
#include "avg_pooling_layer.hpp"

void avg_pooling_layer_top(
  stream_t(avg_pooling_layer_data_t) in[AVG_POOLING_LAYER_COARSE],
  stream_t(avg_pooling_layer_data_t) out[AVG_POOLING_LAYER_COARSE],
  int mode
)
{
#pragma HLS DATAFLOW
#pragma HLS INTERFACE ap_ctrl_chain port=return

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    avg_pooling_layer(in,out,mode);

}