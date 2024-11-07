#include "split_layer_tb.hpp"
#include "split_layer.hpp"

void split_layer_top(
  stream_t(split_layer_data_t) in[SPLIT_LAYER_COARSE],
  stream_t(split_layer_data_t) out_1[SPLIT_LAYER_COARSE],
  stream_t(split_layer_data_t) out_2[SPLIT_LAYER_COARSE], 
  int mode
)
{

#pragma HLS DATAFLOW
#pragma HLS INTERFACE ap_ctrl_chain port=return

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out_1
#pragma HLS STREAM variable=out_2

    split_layer(in,out_1,out_2,mode);

}