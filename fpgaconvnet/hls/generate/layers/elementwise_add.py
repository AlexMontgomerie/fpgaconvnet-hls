# import modules 
import fpgaconvnet.hls.generate.modules.elementwise_add as generate_elementwise_add

elementwise_add_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#include "elementwise_add.hpp"

#define name        {name}
#define NAME        {NAME}
#define {NAME}_ID   {id}

#define {NAME}_BATCH_SIZE   {batch_size}
#define {NAME}_ROWS         {rows}
#define {NAME}_COLS         {cols}
#define {NAME}_CHANNELS     {channels}
#define {NAME}_COARSE       {coarse}

#define {NAME}_COARSE_IN    {NAME}_COARSE
#define {NAME}_COARSE_OUT   {NAME}_COARSE

#define {NAME}_ROWS_OUT     {rows_out}
#define {NAME}_COLS_OUT     {cols_out}
#define {NAME}_CHANNELS_OUT {channels_out}

#define {NAME}_ELEMENTWISE_ADD_BATCH_SIZE   {batch_size}
#define {NAME}_ELEMENTWISE_ADD_ROWS         {rows}
#define {NAME}_ELEMENTWISE_ADD_COLS         {cols}
#define {NAME}_ELEMENTWISE_ADD_CHANNELS     {channels_per_module}

typedef ap_fixed<{data_width},{data_int_width},AP_RND> {name}_data_t;
typedef {name}_data_t {name}_input_t;
typedef {name}_data_t {name}_output_t;

/**
  * FUNCTION DEFINITION
  */
  
void {name}(
    stream_t({name}_data_t) in_1[{NAME}_COARSE],
    stream_t({name}_data_t) in_2[{NAME}_COARSE],
    stream_t({name}_data_t) out[{NAME}_COARSE],
    int mode
);

#undef name
#undef NAME
#endif
"""

elementwise_add_layer_template_src = """#include "{name}.hpp"

void {name}_elementwise_add(
    stream_t({name}_data_t) &in_1,
    stream_t({name}_data_t) &in_2,
    stream_t({name}_data_t) &out
) {{

#pragma HLS INLINE OFF
{elementwise_add}
}}

void {name}(
    stream_t({name}_data_t) in_1[{NAME}_COARSE],
    stream_t({name}_data_t) in_2[{NAME}_COARSE],
    stream_t({name}_data_t) out[{NAME}_COARSE],
    int mode
) 
{{
    
#pragma HLS INLINE OFF

// depth may need to be adjusted to avoid deadlocks
#pragma HLS STREAM variable=in_1 depth=16284
#pragma HLS STREAM variable=in_2 depth=16284
#pragma HLS STREAM variable=out 

#pragma HLS ARRAY_PARTITION variable=in_1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=in_2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

#pragma HLS DATAFLOW

    for (unsigned int coarse_index=0; coarse_index<{NAME}_COARSE; coarse_index++)
    {{
#pragma HLS unroll
        {name}_elementwise_add(in_1[coarse_index], in_2[coarse_index], out[coarse_index]);
    }}
}}

"""

def gen_elementwise_add_layer(name,param,src_path,header_path):
    
    # ELEMENTWISE_ADD MODULE INIT
    elementwise_add = generate_elementwise_add.gen_elementwise_add_module(
        name+"_elementwise_add",
        "in_1",
        "in_2",
        "out",
        elementwise_add_t="data_t",
        indent=4
    )
    
    # src
    elementwise_add_layer_src = elementwise_add_layer_template_src.format(
        name = name, 
        NAME = name.upper(),
        # buffer_depth=max(param['buffer_depth'],2),
        elementwise_add = elementwise_add
    )
    
    # header
    elementwise_add_layer_header = elementwise_add_layer_template_header.format(
        name = name,
        NAME = name.upper(),
        id = 0, #param['id'],
        batch_size = param['batch_size'],
        rows = param['rows_in'],
        cols = param['cols_in'],
        channels = param['channels_in'],
        channels_per_module = param['channels_in']//param['coarse_in'],
        coarse = param['coarse_in'],
        rows_out = param['rows_out'],
        cols_out = param['cols_out'],
        channels_out = param['channels_out'],
        data_width = param['data_t']['width'],
        data_int_width = param['data_t']['width']-param['data_t']['binary_point']
        # data_width = 16,
        # data_int_width = 8
    )
    
    # write source file 
    with open(src_path,'w') as src_file:
        src_file.write(elementwise_add_layer_src)
        
    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(elementwise_add_layer_header)
        
    return
        
        