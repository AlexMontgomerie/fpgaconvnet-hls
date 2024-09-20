import fpgaconvnet.hls.generate.modules.fork as generate_fork

split_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#include "fork.hpp"

#define name        {name}
#define NAME        {NAME}
#define {NAME}_ID   {id}

#define {NAME}_BATCH_SIZE   {batch_size}
#define {NAME}_ROWS         {rows}
#define {NAME}_COLS         {cols}
#define {NAME}_CHANNELS     {channels}
#define {NAME}_COARSE       {coarse}
#define {NAME}_KERNEL_SIZE_X 1
#define {NAME}_KERNEL_SIZE_Y 1

#define {NAME}_COARSE_IN    {NAME}_COARSE
#define {NAME}_COARSE_OUT   {NAME}_COARSE

#define {NAME}_ROWS_OUT     {rows_out}
#define {NAME}_COLS_OUT     {cols_out}
#define {NAME}_CHANNELS_OUT {channels_out}

#define {NAME}_FORK_BATCH_SIZE    {batch_size}
#define {NAME}_FORK_ROWS          {rows}
#define {NAME}_FORK_COLS          {cols}
#define {NAME}_FORK_CHANNELS      {channels_per_module}
#define {NAME}_FORK_COARSE        2
#define {NAME}_FORK_KERNEL_SIZE_X 1
#define {NAME}_FORK_KERNEL_SIZE_Y 1

typedef ap_fixed<{data_width},{data_int_width},AP_RND> {name}_data_t;
typedef {name}_data_t {name}_input_t;
typedef {name}_data_t {name}_output_t;

/**
 * FUNCTION DEFINITION
 */

void {name}(
    stream_t({name}_data_t) in[{NAME}_COARSE],
    stream_t({name}_data_t) out_1[{NAME}_COARSE],
    stream_t({name}_data_t) out_2[{NAME}_COARSE],
    int mode
);

#undef name
#undef NAME
#endif
"""
    
    
split_layer_template_src = """#include "{name}.hpp"

void {name}_fork(
#if {NAME}_KERNEL_SIZE_X == 1 && {NAME}_KERNEL_SIZE_Y == 1
    stream_t({name}_input_t)  &in,
    stream_t({name}_output_t) out[{NAME}_COARSE_OUT]
#else
    stream_t({name}_input_t)  in[{NAME}_KERNEL_SIZE_X][{NAME}_KERNEL_SIZE_Y],
    stream_t({name}_output_t) out[{NAME}_COARSE_OUT][{NAME}_KERNEL_SIZE_X][{NAME}_KERNEL_SIZE_Y]
#endif
) {{

#pragma HLS INLINE OFF
{fork}
}}

void {name}(
    stream_t({name}_data_t) in[{NAME}_COARSE],
    stream_t({name}_data_t) out_1[{NAME}_COARSE],
    stream_t({name}_data_t) out_2[{NAME}_COARSE],
    int mode
) {{
    
#pragma HLS INLINE OFF

#pragma HLS STREAM variable=in depth={buffer_depth}
#pragma HLS STREAM variable=out_1 
#pragma HLS STREAM variable=out_2

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out_1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=out_2 complete dim=0

#pragma HLS DATAFLOW

    stream_t({name}_input_t) fork_out[{NAME}_COARSE_IN][{NAME}_FORK_COARSE];
    #pragma HLS STREAM variable=fork_out
    #pragma HLS ARRAY_PARTITION variable=fork_out complete dim=0

    for(unsigned int coarse_index=0; coarse_index<{NAME}_COARSE; coarse_index++)
    {{
#pragma HLS unroll
        {name}_fork(in[coarse_index], fork_out[coarse_index]);
    }}
    
    for (unsigned long pixel_index=0; pixel_index< DIVIDE({NAME}_BATCH_SIZE*{NAME}_ROWS*{NAME}_COLS*{NAME}_CHANNELS,{NAME}_COARSE); pixel_index++)
    {{
        #pragma HLS PIPELINE II=1
        for(unsigned int coarse_index=0; coarse_index<{NAME}_COARSE; coarse_index++)
        {{
            #pragma HLS unroll
            {name}_data_t tmp_1 = fork_out[coarse_index][0].read();
            {name}_data_t tmp_2 = fork_out[coarse_index][1].read();
            out_1[coarse_index].write(tmp_1);
            out_2[coarse_index].write(tmp_2);
        }}  
    }}
}}

"""
    
    
def gen_split_layer(name,param,src_path,header_path):
    
    # FORK MODULE INIT
    fork = generate_fork.gen_fork_module(
        name+"_fork",
        "in",
        "out",
        fork_t="data_t",
        indent=4
    )
    
    # src 
    split_layer_src = split_layer_template_src.format(
        name=name,
        NAME=name.upper(),
        buffer_depth=max(param['buffer_depth'],2),
        # buffer_depth=2,
        fork=fork
    )
    
    # header
    split_layer_header = split_layer_template_header.format(
        name                =name,
        NAME                =name.upper(),
        id                  =0, # param['id'],
        batch_size          =param['batch_size'],
        rows                =param['rows_in'],
        cols                =param['cols_in'],
        channels            =param['channels_in'],
        channels_per_module =param['channels_in']//param['coarse_in'],
        coarse              =param['coarse_in'],
        rows_out            =param['rows_out'],
        cols_out            =param['cols_out'],
        channels_out        =param['channels_out'],
        data_width          =param['data_t']['width'],
        data_int_width      =(param['data_t']['width']-param['data_t']['binary_point'])
        # data_width          =16,
        # data_int_width      =8
    )
    
    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(split_layer_src)
        
    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(split_layer_header)
        
    return 
