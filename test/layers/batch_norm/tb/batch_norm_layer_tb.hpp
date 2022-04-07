#ifndef BATCH_NORM_LAYER_TB_HPP_
#define BATCH_NORM_LAYER_TB_HPP_

#include "common.hpp"
#include "batch_norm_layer.hpp"
#include "batch_norm_layer_param.hpp"

void batch_norm_layer_top(
  stream_t(data_t) in[BATCH_NORM_LAYER_COARSE],
  stream_t(data_t) out[BATCH_NORM_LAYER_COARSE],
  int mode
);

#endif
