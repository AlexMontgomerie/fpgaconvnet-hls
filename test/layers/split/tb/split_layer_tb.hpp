#ifndef SPLIT_LAYER_TB_HPP_
#define SPLIT_LAYER_TB_HPP_

#include "common.hpp"
#include "split_layer.hpp"
#include "split_layer_param.hpp"

void split_layer_top(
  stream_t(split_layer_data_t) in[SPLIT_LAYER_COARSE],
  stream_t(split_layer_data_t) out_1[SPLIT_LAYER_COARSE],
  stream_t(split_layer_data_t) out_2[SPLIT_LAYER_COARSE], 
  int mode
);

#endif 