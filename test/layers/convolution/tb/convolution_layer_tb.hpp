#ifndef CONVOLUTION_LAYER_TB_HPP_
#define CONVOLUTION_LAYER_TB_HPP_

#include "common.hpp"
#include "convolution_layer.hpp"
#include "convolution_layer_param.hpp"

void convolution_layer_top(
  stream_t(convolution_layer_input_t) in[CONVOLUTION_LAYER_COARSE_IN*CONVOLUTION_LAYER_COARSE_GROUP],
  stream_t(convolution_layer_output_t) out[CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_COARSE_GROUP],
  int mode
);

#endif
