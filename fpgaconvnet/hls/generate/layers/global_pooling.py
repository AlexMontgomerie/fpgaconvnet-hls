import fpgaconvnet.hls.generate.modules.global_pool as generate_global_pool

global_pooling_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#include "global_pool.hpp"

#define name        {name}
#define NAME        {NAME}
#define {NAME}_ID   {id}

#define {NAME}_BATCH_SIZE    {batch_size}
#define {NAME}_ROWS          {rows}
#define {NAME}_COLS          {cols}
#define {NAME}_CHANNELS      {channels}
#define {NAME}_COARSE        {coarse}

#define {NAME}_COARSE_IN    {NAME}_COARSE
#define {NAME}_COARSE_OUT   {NAME}_COARSE

#define {NAME}_ROWS_OUT     {rows_out}
#define {NAME}_COLS_OUT     {cols_out}
#define {NAME}_CHANNELS_OUT {channels_out}

#define {NAME}_GLOBAL_POOL_BATCH_SIZE    {batch_size}
#define {NAME}_GLOBAL_POOL_ROWS          {rows}
#define {NAME}_GLOBAL_POOL_COLS          {cols}
#define {NAME}_GLOBAL_POOL_CHANNELS      {channels_per_module}

typedef ap_fixed<{data_width},{data_int_width},AP_RND> {name}_data_t;
typedef {name}_data_t {name}_input_t;
typedef {name}_data_t {name}_output_t;

/**
  * FUNCTION DEFINITION
  */
  
void {name}(
    stream_t({name}_data_t) in[{NAME}_COARSE],
    stream_t({name}_data_t) out[{NAME}_COARSE], 
    int mode
);

#undef name
#undef NAME
#endif
"""

global_pppppooling_layer_template_src = """#include "{name}.hpp"

void {name}_global_pool(
    stream_t({name}_data_t) &in,
    stream_t({name}_data_t) &out
) {{
#pragma HLS INLINE OFF
{global_pool}
}}

void {name}(
    stream_t({name}_data_t) in[{NAME}_COARSE],
    stream_t({name}_data_t) out[{NAME}_COARSE], 
    int mode
) {{
    #pragma HLS INLINE OFF
    
    #pragma HLS STREAM variable=in depth={buffer_depth}
    #pragma HLS STREAM variable=out
    
    #pragma HLS ARRAY_PARTITION variable=in complete dim=0
    #pragma HLS ARRAY_PARTITION variable=out complete dim=0
    
    #pragma HLS DATAFLOW
    
    for (unsigned int coarse_index=0; coarse_index<{NAME}_COARSE; coarse_index++) 
    {{
#pragma HLS UNROLL
        {name}_global_pool(in[coarse_index], out[coarse_index]);
    }}
}}
"""

def gen_global_pooling_layer(name,param,src_path,header_path): 
    
    # GLOBAL POOL MODULE INIT
    global_pool = generate_global_pool.gen_global_pool_module(
        name+"_global_pool",
        "in",
        "out",
        global_pool_t=f"{name}_data_t",
        indent=4
    )
    
    # src
    global_pooling_layer_src = global_pppppooling_layer_template_src.format(
        name=name,
        NAME=name.upper(),
        buffer_depth=max(param['buffer_depth'],2),
        global_pool=global_pool
    )
    
    global_pooling_layer_header = global_pooling_layer_template_header.format(
        name                =name,
        NAME                =name.upper(),
        id                  =0, # param['id'],
        batch_size          =param['batch_size'],
        rows                =param['rows_in'],
        cols                =param['cols_in'],
        channels            =param['channels_in'],
        channels_per_module =param['channels_in']//param['coarse'],
        coarse              =param['coarse'],
        rows_out            =param['rows_out'],
        cols_out            =param['cols_out'],
        channels_out        =param['channels_out'],
        data_width          =param['data_t']['width'],
        data_int_width      =param['data_t']['width']-param['data_t']['binary_point']
    )
    
    # write source file 
    with open(src_path, 'w') as src_file: 
        src_file.write(global_pooling_layer_src)
        
    # write header file
    with open(header_path, 'w') as header_file: 
        header_file.write(global_pooling_layer_header)
        
    return 