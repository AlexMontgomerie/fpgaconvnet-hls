#ifndef GLOBAL_POOLING_LAYER_TB_HPP
#define GLOBAL_POOLING_LAYER_TB_HPP

#include "common.hpp"
#include "global_pooling_layer.hpp"
#include "global_pooling_layer_param.hpp"

void global_pooling_layer_top(
  stream_t(global_pooling_layer_data_t) in[GLOBAL_POOLING_LAYER_COARSE],
  stream_t(global_pooling_layer_data_t) out[GLOBAL_POOLING_LAYER_COARSE],
  int mode
);

#endif 