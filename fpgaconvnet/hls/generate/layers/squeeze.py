# import modules
import os
import math
import shutil
import fpgaconvnet.hls.generate.modules.squeeze as generate_squeeze

squeeze_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#include "squeeze.hpp"

#define {NAME}_BATCH_SIZE   {batch_size}
#define {NAME}_ROWS         {rows_in}
#define {NAME}_COLS         {cols_in}
#define {NAME}_CHANNELS     {channels_in}

#define {NAME}_COARSE_IN    {coarse_in}
#define {NAME}_COARSE_OUT   {coarse_out}

#define {NAME}_ROWS_OUT     {rows_out}
#define {NAME}_COLS_OUT     {cols_out}
#define {NAME}_CHANNELS_OUT {channels_out}

#define {NAME}_SQUEEZE_BATCH_SIZE   {batch_size}
#define {NAME}_SQUEEZE_ROWS         {rows_in}
#define {NAME}_SQUEEZE_COLS         {cols_in}
#define {NAME}_SQUEEZE_CHANNELS     {channels_in}
#define {NAME}_SQUEEZE_CHANNELS_PER_COARSE_IN     {channels_per_coarse_in}
#define {NAME}_SQUEEZE_CHANNELS_PER_COARSE_OUT    {channels_per_coarse_out}
#define {NAME}_SQUEEZE_COARSE_IN    {coarse_in}
#define {NAME}_SQUEEZE_COARSE_OUT   {coarse_out}
#define {NAME}_SQUEEZE_BUFFER_SIZE  {buffer_size}

typedef ap_fixed<{data_width},{data_int_width},AP_RND> {name}_data_t;
typedef {name}_data_t {name}_input_t;
typedef {name}_data_t {name}_output_t;

/**
 * FUNCTION DEFINITION
 */

void {name}(
    stream_t({name}_data_t) in[{NAME}_COARSE_IN],
    stream_t({name}_data_t) out[{NAME}_COARSE_OUT],
    int mode
);

#endif
"""

squeeze_layer_template_src = """#include "{name}.hpp"

void {name}(
    stream_t({name}_data_t) in[{NAME}_COARSE_IN],
    stream_t({name}_data_t) out[{NAME}_COARSE_OUT],
    int mode
)
{{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=in depth={buffer_depth}
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

#pragma HLS DATAFLOW

{squeeze}

}}

"""

def lcm(a, b):
    return abs(a*b) // math.gcd(a, b)

def gen_squeeze_layer(name,param,src_path,header_path):

    # BATCH NORM MODULE INIT
    squeeze = generate_squeeze.gen_squeeze_module(
        name+"_squeeze",
        "in",
        "out",
        squeeze_t=f"{name}_data_t",
        indent=4
    )

    # src
    squeeze_layer_src = squeeze_layer_template_src.format(
        name  =name,
        NAME  =name.upper(),
        buffer_depth=max(param['buffer_depth'],2),
        squeeze  =squeeze
    )

    # header
    squeeze_layer_header = squeeze_layer_template_header.format(
        name                =name,
        NAME                =name.upper(),
        id                  =0, # param['id'],
        batch_size          =param['batch_size'],
        rows_in             =param['rows_in'],
        cols_in             =param['cols_in'],
        channels_in         =param['channels_in'],
        channels_per_coarse_in  =param['channels_in']//param["coarse_in"],
        channels_per_coarse_out =param['channels_in']//param["coarse_out"],
        coarse_in           =param['coarse_in'],
        coarse_out          =param['coarse_out'],
        rows_out            =param['rows_out'],
        cols_out            =param['cols_out'],
        channels_out        =param['channels_out'],
        data_width          =param['data_t']['width'],
        data_int_width      =(param['data_t']['width']-param['data_t']['binary_point']),
        buffer_size         =lcm(param['coarse_in'],param['coarse_out']),
    )

    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(squeeze_layer_src)

    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(squeeze_layer_header)

    return
