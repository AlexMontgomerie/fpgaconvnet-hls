import fpgaconvnet.hls.generate.modules.sliding_window as generate_sliding_window
import fpgaconvnet.hls.generate.modules.avg_pool as generate_avg_pool

avg_pooling_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#include "sliding_window.hpp"
#include "avg_pool.hpp"

#define name        {name}
#define NAME        {NAME}
#define {NAME}_ID   {id}

#define {NAME}_BATCH_SIZE    {batch_size}
#define {NAME}_ROWS          {rows}
#define {NAME}_COLS          {cols}
#define {NAME}_CHANNELS      {channels}
#define {NAME}_COARSE        {coarse}
#define {NAME}_KERNEL_SIZE_X {kernel_size_x}
#define {NAME}_KERNEL_SIZE_Y {kernel_size_y}
#define {NAME}_STRIDE_X      {stride_x}
#define {NAME}_STRIDE_Y      {stride_y}
#define {NAME}_FINE          {fine}

#define {NAME}_COARSE_IN    {NAME}_COARSE
#define {NAME}_COARSE_OUT   {NAME}_COARSE

#define {NAME}_ROWS_OUT     {rows_out}
#define {NAME}_COLS_OUT     {cols_out}
#define {NAME}_CHANNELS_OUT {channels_out}

// define the data type
typedef ap_fixed<{data_width},{data_int_width},AP_RND> {name}_data_t;
typedef {name}_data_t {name}_input_t;
typedef {name}_data_t {name}_output_t;

// SLIDING WINDOW
#define {NAME}_SLIDING_WINDOW_BATCH_SIZE    {batch_size}
#define {NAME}_SLIDING_WINDOW_ROWS          {rows}
#define {NAME}_SLIDING_WINDOW_COLS          {cols}
#define {NAME}_SLIDING_WINDOW_CHANNELS      {channels_per_module}
#define {NAME}_SLIDING_WINDOW_KERNEL_SIZE_X {kernel_size_x}
#define {NAME}_SLIDING_WINDOW_KERNEL_SIZE_Y {kernel_size_y}
#define {NAME}_SLIDING_WINDOW_STRIDE_X      {stride_x}
#define {NAME}_SLIDING_WINDOW_STRIDE_Y      {stride_y}
#define {NAME}_SLIDING_WINDOW_PAD_LEFT      {pad_left}
#define {NAME}_SLIDING_WINDOW_PAD_RIGHT     {pad_right}
#define {NAME}_SLIDING_WINDOW_PAD_TOP       {pad_top}
#define {NAME}_SLIDING_WINDOW_PAD_BOTTOM    {pad_bottom}

// AVG_POOL
#define {NAME}_AVG_POOL_BATCH_SIZE   {batch_size}
#define {NAME}_AVG_POOL_ROWS         {rows_out}
#define {NAME}_AVG_POOL_COLS         {cols_out}
#define {NAME}_AVG_POOL_CHANNELS     {channels_per_module}
#define {NAME}_AVG_POOL_KERNEL_SIZE_X {kernel_size_x}
#define {NAME}_AVG_POOL_KERNEL_SIZE_Y {kernel_size_y}
#define {NAME}_AVG_POOL_FINE         {fine}

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

avg_pooling_layer_template_src = """#include "{name}.hpp"

void {name}_sliding_window(
    stream_t({name}_data_t) &in,
    stream_t({name}_data_t) out[{NAME}_KERNEL_SIZE_X][{NAME}_KERNEL_SIZE_Y]
) {{
#pragma HLS INLINE OFF
{sliding_window}
}}

void {name}_avg_pool(
    stream_t({name}_data_t) in[{NAME}_KERNEL_SIZE_X][{NAME}_KERNEL_SIZE_Y],
    stream_t({name}_data_t) &out
) {{
#pragma HLS INLINE OFF
{avg_pool}
}}

void {name}(
    stream_t({name}_data_t) in[{NAME}_COARSE],
    stream_t({name}_data_t) out[{NAME}_COARSE],
    int mode
)
{{
    
#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

#pragma HLS STREAM variable=in depth={buffer_depth}
#pragma HLS STREAM variable=out 

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    stream_t({name}_data_t) sw_out[{NAME}_COARSE][{NAME}_KERNEL_SIZE_X][{NAME}_KERNEL_SIZE_Y]; //sliding window output
    
#pragma HLS STREAM variable=sw_out
#pragma HLS ARRAY_PARTITION variable=sw_out complete dim=0

    for(unsigned int coarse_index=0; coarse_index<{NAME}_COARSE; coarse_index++)
    {{
#pragma HLS UNROLL
        {name}_sliding_window(in[coarse_index],sw_out[coarse_index]);
        {name}_avg_pool(sw_out[coarse_index],out[coarse_index]);
    }}
}}

"""

def gen_avg_pooling_layer(name,param,src_path,header_path):
    
    # get sliding window type
    single_channel = True if param['channels_in'] == 1 else False
    
    # SLIDING WINDOW MODULE INIT
    sliding_window = generate_sliding_window.gen_sliding_window_module(
        name+"_sliding_window", 
        "in", "out",
        sliding_window_t=f"{name}_data_t",
        indent=4
    )
    
    # AVG_POOL MODULE INIT
    avg_pool = generate_avg_pool.gen_avg_pool_module(
        name+"_avg_pool",
        "in", 
        "out",
        avg_pool_t=f"{name}_data_t",
        indent=4
    )
    
    # src
    avg_pooling_layer_src = avg_pooling_layer_template_src.format(
        name            =name,
        NAME            =name.upper(),
        # buffer_depth=max(param['buffer_depth'],2),
        buffer_depth    =2,
        sliding_window  =sliding_window,
        avg_pool            =avg_pool
    )
    
    # header
    avg_pooling_layer_header = avg_pooling_layer_template_header.format(
        name                =name,
        NAME                =name.upper(),
        id                  =0, # param['id'],
        batch_size          =param['batch_size'],
        rows                =param['rows_in'],
        cols                =param['cols_in'],
        channels            =param['channels_in'],
        channels_per_module =param['channels_in']//param['coarse'],
        coarse              =param['coarse'],
        kernel_size_x       =param['kernel_size'][0],
        kernel_size_y       =param['kernel_size'][1],
        stride_x            =param['stride'][0],
        stride_y            =param['stride'][1],
        pad_left            =param['pad_left'],
        pad_right           =param['pad_right'],
        pad_top             =param['pad_top'],
        pad_bottom          =param['pad_bottom'],
        fine                =param['fine'] if 'fine' in param else "1",
        rows_out            =param['rows_out'],
        cols_out            =param['cols_out'],
        channels_out        =param['channels_out'],
        data_width          =param['data_t']['width'],
        data_int_width      =(param['data_t']['width']-param['data_t']['binary_point'])
    )
    
    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(avg_pooling_layer_src)
        
    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(avg_pooling_layer_header)
        
    return 



