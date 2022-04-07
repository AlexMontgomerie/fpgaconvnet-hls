#include "squeeze_layer_tb.hpp"
#include "squeeze_layer.hpp"

void squeeze_layer_top(
  stream_t(squeeze_layer_data_t) in[SQUEEZE_LAYER_COARSE_IN],
  stream_t(squeeze_layer_data_t) out[SQUEEZE_LAYER_COARSE_OUT],
  int mode
)
{

#pragma HLS DATAFLOW
#pragma HLS INTERFACE ap_ctrl_chain port=return

//#pragma HLS INTERFACE axis port=in
//#pragma HLS INTERFACE axis port=out
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    squeeze_layer(in,out,mode);

}
