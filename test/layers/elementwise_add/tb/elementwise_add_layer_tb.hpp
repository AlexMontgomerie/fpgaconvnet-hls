#ifndef ELEMENTWISE_ADD_LAYER_TB_HPP_
#define ELEMENTWISE_ADD_LAYER_TB_HPP_

#include "common.hpp"
#include "elementwise_add_layer.hpp"
#include "elementwise_add_layer_param.hpp"

void elementwise_add_layer_top(
  stream_t(elementwise_add_layer_data_t) in1[ELEMENTWISE_ADD_LAYER_COARSE],
  stream_t(elementwise_add_layer_data_t) in2[ELEMENTWISE_ADD_LAYER_COARSE],
  stream_t(elementwise_add_layer_data_t) out[ELEMENTWISE_ADD_LAYER_COARSE],
  int mode
);

#endif 