#ifndef SQUEEZE_LAYER_TB_HPP_
#define SQUEEZE_LAYER_TB_HPP_

#include "common.hpp"
#include "squeeze_layer.hpp"
#include "squeeze_layer_param.hpp"

void squeeze_layer_top(
  stream_t(squeeze_layer_data_t) in[SQUEEZE_LAYER_COARSE_IN],
  stream_t(squeeze_layer_data_t) out[SQUEEZE_LAYER_COARSE_OUT],
  int mode
);

#endif
