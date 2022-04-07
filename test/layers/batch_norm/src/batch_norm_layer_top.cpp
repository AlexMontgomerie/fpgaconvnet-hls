#include "batch_norm_layer_tb.hpp"
#include "batch_norm_layer.hpp"

const static data_t scale[BATCH_NORM_LAYER_COARSE][CHANNELS_3D(BATCH_NORM_LAYER_CHANNELS,BATCH_NORM_LAYER_COARSE)] = {
#include "scale.csv"
};

const static data_t shift[BATCH_NORM_LAYER_COARSE][CHANNELS_3D(BATCH_NORM_LAYER_CHANNELS,BATCH_NORM_LAYER_COARSE)] = {
#include "shift.csv"
};

void batch_norm_layer_top(
  stream_t(data_t) in[BATCH_NORM_LAYER_COARSE_IN],
  stream_t(data_t) out[BATCH_NORM_LAYER_COARSE_OUT],
  int mode
)
{
#pragma HLS DATAFLOW

//#pragma HLS INTERFACE axis port=in
//#pragma HLS INTERFACE axis port=out
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

DO_PRAGMA(HLS ARRAY_PARTITION variable=scale block factor=BATCH_NORM_LAYER_COARSE  dim=1)
#pragma HLS RESOURCE variable=scale core=RAM_2P_BRAM

DO_PRAGMA(HLS ARRAY_PARTITION variable=shift block factor=BATCH_NORM_LAYER_COARSE  dim=1)
#pragma HLS RESOURCE variable=shift core=RAM_2P_BRAM

    batch_norm_layer(in,scale,shift,out,mode);

}
