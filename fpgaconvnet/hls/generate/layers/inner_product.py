# import modules
import os
import shutil

import fpgaconvnet.hls.generate.modules.sliding_window as generate_sliding_window
import fpgaconvnet.hls.generate.modules.fork as generate_fork
import fpgaconvnet.hls.generate.modules.conv as generate_conv
import fpgaconvnet.hls.generate.modules.accum as generate_accum
import fpgaconvnet.hls.generate.modules.glue as generate_glue
import fpgaconvnet.hls.generate.modules.bias as generate_bias

inner_product_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#define name        {name}
#define NAME        {NAME}
#define {NAME}_ID   {id}

#include "fork.hpp"
#include "conv.hpp"
#include "accum.hpp"
#include "glue.hpp"
#include "bias.hpp"

#define {NAME}_BATCH_SIZE    {batch_size}
#define {NAME}_ROWS          {rows}
#define {NAME}_COLS          {cols}
#define {NAME}_CHANNELS      {channels}
#define {NAME}_FILTERS       {filters}
#define {NAME}_COARSE_IN     {coarse_in}
#define {NAME}_COARSE_OUT    {coarse_out}
#define {NAME}_COARSE_GROUP  1
#define {NAME}_KERNEL_SIZE_X 1
#define {NAME}_KERNEL_SIZE_Y 1

#define {NAME}_HAS_BIAS {has_bias}

// coefficients
#define {NAME}_WEIGHTS {NAME}_ROWS*{NAME}_COLS*{NAME}_CHANNELS*{NAME}_FILTERS

// dimensions out
#define {NAME}_ROWS_OUT     {rows_out}
#define {NAME}_COLS_OUT     {cols_out}
#define {NAME}_CHANNELS_OUT {channels_out}

// define data types
typedef ap_fixed<{input_width},{input_int_width},AP_RND>    {name}_input_t;
typedef ap_fixed<{output_width},{output_int_width},AP_RND>  {name}_output_t;
typedef ap_fixed<{acc_width},{acc_int_width},AP_RND>        {name}_acc_t;
typedef ap_fixed<{weight_width},{weight_int_width},AP_RND>  {name}_weight_t;
typedef ap_fixed<{biases_width},{biases_int_width},AP_RND>  {name}_biases_t;

// FORK
#define {NAME}_FORK_BATCH_SIZE   {batch_size}
#define {NAME}_FORK_ROWS         1
#define {NAME}_FORK_COLS         1
#define {NAME}_FORK_CHANNELS     {channels_per_module}
#define {NAME}_FORK_COARSE       {coarse_out}
#define {NAME}_FORK_KERNEL_SIZE_X 1
#define {NAME}_FORK_KERNEL_SIZE_Y 1

// CONV
#define {NAME}_CONV_BATCH_SIZE   {batch_size}
#define {NAME}_CONV_ROWS         1
#define {NAME}_CONV_COLS         1
#define {NAME}_CONV_CHANNELS     {channels_per_module}
#define {NAME}_CONV_FILTERS      {filters_per_module}
#define {NAME}_CONV_CHANNELS_PER_GROUP {channels_per_module}
#define {NAME}_CONV_FILTERS_PER_GROUP  {filters_per_module}
#define {NAME}_CONV_GROUPS       1
#define {NAME}_CONV_KERNEL_SIZE_X 1
#define {NAME}_CONV_KERNEL_SIZE_Y 1
#define {NAME}_CONV_FINE         1
#define {NAME}_CONV_INTERVAL     1

// ACCUM
#define {NAME}_ACCUM_BATCH_SIZE         {batch_size}
#define {NAME}_ACCUM_ROWS               1
#define {NAME}_ACCUM_COLS               1
#define {NAME}_ACCUM_GROUPS             1
#define {NAME}_ACCUM_CHANNELS           {channels_per_module}
#define {NAME}_ACCUM_FILTERS            {filters_per_module}
#define {NAME}_ACCUM_CHANNELS_PER_GROUP {channels_per_module}
#define {NAME}_ACCUM_FILTERS_PER_GROUP  {filters_per_module}

// GLUE
#define {NAME}_GLUE_BATCH_SIZE   {batch_size}
#define {NAME}_GLUE_ROWS         1
#define {NAME}_GLUE_COLS         1
#define {NAME}_GLUE_FILTERS      {channels_out}
#define {NAME}_GLUE_FILTERS_PER_COARSE  {filters_per_module}
#define {NAME}_GLUE_COARSE_IN    {coarse_in}
#define {NAME}_GLUE_COARSE_OUT   {coarse_out}
#define {NAME}_GLUE_COARSE_GROUP 1

// BIAS
#define {NAME}_BIAS_BATCH_SIZE   {batch_size}
#define {NAME}_BIAS_ROWS         1
#define {NAME}_BIAS_COLS         1
//#define {NAME}_BIAS_FILTERS      DIVIDE({NAME}_FILTERS, {NAME}_COARSE_OUT*{NAME}_WR_FACTOR)
#define {NAME}_BIAS_FILTERS      {filters_per_module}

/**
 * FUNCTION DEFINITION
 */

void {name}(
    const {name}_weight_t weights[{NAME}_COARSE_IN][{NAME}_COARSE_OUT][DIVIDE({NAME}_WEIGHTS,{NAME}_COARSE_IN*{NAME}_COARSE_OUT)][1][1],
#if {NAME}_HAS_BIAS == 1
    const {name}_biases_t biases[{NAME}_COARSE_OUT][{NAME}_BIAS_FILTERS],
#endif
    stream_t({name}_input_t) in[{NAME}_COARSE_IN],
    stream_t({name}_output_t) out[{NAME}_COARSE_OUT],
    int mode
);

#endif
#undef name
#undef NAME
"""

inner_product_layer_template_src = """#include "{name}.hpp"

void {name}_fork(
    stream_t({name}_input_t)  &in,
    stream_t({name}_output_t) out[{NAME}_COARSE_OUT]
) {{

#pragma HLS INLINE OFF
{fork}
}}

void {name}_conv(
    const {name}_weight_t weights[DIVIDE({NAME}_WEIGHTS,{NAME}_COARSE_IN*{NAME}_COARSE_OUT)][1][1],
    stream_t({name}_input_t) &in,
    stream_t({name}_acc_t) &out
) {{

#pragma HLS INLINE OFF
{conv}
}}

void {name}_accum(
    stream_t({name}_acc_t) &in,
    stream_t({name}_acc_t) &out
) {{

#pragma HLS INLINE OFF
{accum}
}}

void {name}_glue(
    stream_t({name}_acc_t) in[{NAME}_COARSE_IN][{NAME}_COARSE_OUT],
    stream_t({name}_output_t) out[{NAME}_COARSE_OUT]
) {{

#pragma HLS INLINE OFF
{glue}
}}

void {name}_bias(
    const {name}_biases_t biases[{NAME}_BIAS_FILTERS],
    stream_t({name}_output_t) &in,
    stream_t({name}_output_t) &out
) {{

#pragma HLS INLINE OFF
{bias}
}}

void {name}(
    const {name}_weight_t weights[{NAME}_COARSE_IN][{NAME}_COARSE_OUT][DIVIDE({NAME}_WEIGHTS,{NAME}_COARSE_IN*{NAME}_COARSE_OUT)][1][1],
#if {NAME}_HAS_BIAS == 1
    const {name}_biases_t biases[{NAME}_COARSE_OUT][{NAME}_BIAS_FILTERS],
#endif
    stream_t({name}_input_t) in[{NAME}_COARSE_IN],
    stream_t({name}_output_t) out[{NAME}_COARSE_OUT],
    int mode
)
{{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=in depth={buffer_depth}
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

#pragma HLS DATAFLOW

    stream_t({name}_input_t) fork_out[{NAME}_COARSE_IN][{NAME}_COARSE_OUT];
    #pragma HLS STREAM variable=fork_out
    #pragma HLS ARRAY_PARTITION variable=fork_out complete dim=0

    stream_t({name}_acc_t) conv_out[{NAME}_COARSE_IN][{NAME}_COARSE_OUT];
    #pragma HLS STREAM variable=conv_out
    #pragma HLS ARRAY_PARTITION variable=conv_out complete dim=0

#if {NAME}_ACCUM_CHANNELS > 1
    stream_t({name}_acc_t) accum_out[{NAME}_COARSE_IN][{NAME}_COARSE_OUT];
    #pragma HLS STREAM variable=accum_out
    #pragma HLS ARRAY_PARTITION variable=accum_out complete dim=0
#endif

#if {NAME}_HAS_BIAS == 1
    stream_t({name}_output_t) glue_out[{NAME}_COARSE_OUT];
    #pragma HLS STREAM variable=glue_out
    #pragma HLS ARRAY_PARTITION variable=glue_out complete dim=0
#endif

    {name}_coarse_in_loop: for(int i=0;i<{NAME}_COARSE_IN;i++) {{
        #pragma HLS UNROLL

        {name}_fork(in[i], fork_out[i]);

        {name}_coarse_out_loop: for(int j=0;j<{NAME}_COARSE_OUT;j++) {{
            #pragma HLS UNROLL
            {name}_conv(weights[i][j], fork_out[i][j], conv_out[i][j]);
#if {NAME}_ACCUM_CHANNELS > 1
            {name}_accum(conv_out[i][j], accum_out[i][j]);
#endif
        }}
    }}

#if {NAME}_ACCUM_CHANNELS > 1
#if {NAME}_HAS_BIAS == 1
    {name}_glue(accum_out, glue_out);
    {name}_coarse_out_bias_loop: for(unsigned int i=0;i<{NAME}_COARSE_OUT;i++) {{
        #pragma HLS unroll
        {name}_bias(biases[i], glue_out[i], out[i]);
    }}
#else
    {name}_glue(accum_out, out);
#endif
#else
#if {NAME}_HAS_BIAS == 1
    {name}_glue(conv_out, glue_out);
    {name}_coarse_out_bias_loop: for(unsigned int i=0;i<{NAME}_COARSE_OUT;i++) {{
        #pragma HLS unroll
        {name}_bias(biases[i], glue_out[i], out[i]);
    }}
#else
    {name}_glue(conv_out, out);
#endif
#endif

}}

"""

def gen_inner_product_layer(name,param,src_path,header_path):

    # FORK MODULE INIT
    fork = generate_fork.gen_fork_module(
        name+"_fork",
        "in", "out",
        fork_t=f"{name}_input_t",
        indent=4
    )

    # CONV MODULE INIT
    conv = generate_conv.gen_conv_module(
        name+"_conv",
        "in", "weights", "out",
        data_t=f"{name}_input_t",
        acc_t=f"{name}_acc_t",
        weight_t=f"{name}_weight_t",
        indent=4
    )

    # ACCUM MODULE INIT
    accum = generate_accum.gen_accum_module(
        name+"_accum",
        "in", "out",
        accum_t=f"{name}_acc_t",
        indent=4
    )

    # GLUE MODULE INIT
    glue = generate_glue.gen_glue_module(
        name+"_glue",
        "in", "out",
        acc_t=f"{name}_acc_t",
        data_t=f"{name}_output_t",
        indent=4
    )

    # BIAS MODULE INIT
    bias = generate_bias.gen_bias_module(
        name+"_bias",
        "in", "biases", "out",
        data_t=f"{name}_output_t",
        biases_t=f"{name}_biases_t",
        indent=4
    )

    # src
    inner_product_layer_src = inner_product_layer_template_src.format(
        name            =name,
        NAME            =name.upper(),
        buffer_depth    =max(param['buffer_depth'],2),
        fork            =fork,
        conv            =conv,
        accum           =accum,
        glue            =glue,
        bias            =bias
    )

    # header
    inner_product_layer_header = inner_product_layer_template_header.format(
        name            =name,
        NAME            =name.upper(),
        id              =0, # param['id'],
        batch_size      =param['batch_size'],
        rows            =param['rows_in'],
        cols            =param['cols_in'],
        channels        =param['channels_in'],
        rows_out        =param['rows_out'],
        cols_out        =param['cols_out'],
        channels_out    =param['channels_out'],
        filters         =param['filters'],
        channels_per_module =param['channels_in']//param['coarse_in'],
        filters_per_module  =param['filters']//param['coarse_out'],
        coarse_in           =param['coarse_in'],
        coarse_out          =param['coarse_out'],
        input_width         =param['input_t']['width'],
        input_int_width     =(param['input_t']['width'] - param['input_t']['binary_point']),
        output_width        =param['output_t']['width'],
        output_int_width    =(param['output_t']['width'] - param['output_t']['binary_point']),
        acc_width           =param['acc_t']['width'],
        acc_int_width       =(param['acc_t']['width'] - param['acc_t']['binary_point']),
        weight_width        =param['weight_t']['width'],
        weight_int_width    =(param['weight_t']['width'] - param['weight_t']['binary_point']),
        biases_width        =param['acc_t']['width'],
        biases_int_width    =(param['acc_t']['width'] - param['acc_t']['binary_point']),
        has_bias            =param['has_bias'],
    )

    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(inner_product_layer_src)

    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(inner_product_layer_header)

    return
