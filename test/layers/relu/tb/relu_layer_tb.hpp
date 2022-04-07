#ifndef RELU_LAYER_TB_HPP_
#define RELU_LAYER_TB_HPP_

#include "common.hpp"
#include "relu_layer.hpp"
#include "relu_layer_param.hpp"

void relu_layer_top(
  stream_t(relu_layer_data_t) in[RELU_LAYER_COARSE],
  stream_t(relu_layer_data_t) out[RELU_LAYER_COARSE],
  int mode
);

#endif
