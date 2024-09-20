#ifndef ELEMENTWISE_MUL_LAYER_TB_HPP_
#define ELEMENTWISE_MUL_LAYER_TB_HPP_

#include "common.hpp"
#include "elementwise_mul_layer.hpp"
#include "elementwise_mul_layer_param.hpp"

void elementwise_mul_layer_top(
  stream_t(elementwise_mul_layer_data_t) in1[ELEMENTWISE_MUL_LAYER_COARSE],
  stream_t(elementwise_mul_layer_data_t) in2[ELEMENTWISE_MUL_LAYER_COARSE],
  stream_t(elementwise_mul_layer_data_t) out[ELEMENTWISE_MUL_LAYER_COARSE],
  int mode
);

#endif