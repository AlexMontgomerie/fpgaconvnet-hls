#ifndef AVG_POOLING_LAYER_TB_HPP
#define AVG_POOLING_LAYER_TB_HPP

#include "common.hpp"
#include "avg_pooling_layer.hpp"
#include "avg_pooling_layer_param.hpp"

void avg_pooling_layer_top(
  stream_t(avg_pooling_layer_data_t) in[AVG_POOLING_LAYER_COARSE],
  stream_t(avg_pooling_layer_data_t) out[AVG_POOLING_LAYER_COARSE],
  int mode
);

#endif