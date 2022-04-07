#include "inner_product_layer_tb.hpp"
#include "inner_product_layer.hpp"

void inner_product_layer_top(
  stream_t(inner_product_layer_input_t) in[INNER_PRODUCT_LAYER_COARSE_IN],
  stream_t(inner_product_layer_output_t) out[INNER_PRODUCT_LAYER_COARSE_OUT],
  int mode
)
{
#pragma HLS DATAFLOW

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    const static inner_product_layer_weight_t weights[INNER_PRODUCT_LAYER_COARSE_IN][INNER_PRODUCT_LAYER_COARSE_OUT][CHANNELS_3D(INNER_PRODUCT_LAYER_ROWS*INNER_PRODUCT_LAYER_COLS*INNER_PRODUCT_LAYER_CHANNELS,INNER_PRODUCT_LAYER_COARSE_IN)*CHANNELS_3D(INNER_PRODUCT_LAYER_FILTERS,INNER_PRODUCT_LAYER_COARSE_OUT)][1][1] = {
#include "weights.csv"
    };

#pragma HLS ARRAY_PARTITION variable=weights complete dim=1
#pragma HLS ARRAY_PARTITION variable=weights complete dim=2
#pragma HLS RESOURCE variable=weights core=RAM_2P_BRAM

    inner_product_layer(weights,in,out,mode);

}
