"""
Header, Source and TestBench templates for partition generation.
"""

network_header_template = """#ifndef {NAME}_TOP_HPP_
#define {NAME}_TOP_HPP_

#include "common.hpp"
{include}
#include "mem_read.hpp"
#include "mem_write.hpp"
#include "wr.hpp"

#define {NAME}_BATCH_SIZE   {batch_size}

#define {NAME}_ROWS_IN      {rows_in}
#define {NAME}_COLS_IN      {cols_in}
#define {NAME}_CHANNELS_IN  {channels_in}

#define {NAME}_ROWS_OUT     {rows_out}
#define {NAME}_COLS_OUT     {cols_out}
#define {NAME}_CHANNELS_OUT {channels_out}

#define {NAME}_STREAMS_IN   {streams_in}
#define {NAME}_STREAMS_OUT  {streams_out}
#define {NAME}_STREAMS_WR   1

#define {NAME}_PORTS        {ports}
#define {NAME}_PORTS_IN     {ports}  //{NAME}_PORTS
#define {NAME}_PORTS_OUT    {ports}  //{NAME}_PORTS
#define {NAME}_PORTS_WR     1 //{NAME}_PORTS

#define {NAME}_WEIGHTS_RELOADING_FACTOR {wr_factor}
#define {NAME}_WEIGHTS_RELOADING_LAYER  {wr_layer}
#define {NAME}_WEIGHTS_RELOADING_FLAG   {wr_flag}

#define {NAME}_SIZE_IN  {NAME}_BATCH_SIZE*{NAME}_ROWS_IN*{NAME}_COLS_IN*DIVIDE({NAME}_CHANNELS_IN,{NAME}_STREAMS_IN)
#define {NAME}_SIZE_OUT {NAME}_BATCH_SIZE*{NAME}_ROWS_OUT*{NAME}_COLS_OUT*DIVIDE({NAME}_CHANNELS_OUT,{NAME}_STREAMS_OUT)*{NAME}_WEIGHTS_RELOADING_FACTOR

typedef {input_layer}_input_t   {name}_input_t;
typedef {output_layer}_output_t {name}_output_t;

#if {NAME}_WEIGHTS_RELOADING_FLAG
#define {NAME}_WR_COARSE_IN       {WR_LAYER}_COARSE_IN
#define {NAME}_WR_COARSE_OUT      {WR_LAYER}_COARSE_OUT
#define {NAME}_WR_COARSE_GROUP    {WR_LAYER}_COARSE_GROUP
#define {NAME}_WR_WEIGHTS         {WR_LAYER}_WEIGHTS
#define {NAME}_WR_KERNEL_SIZE_X   {WR_LAYER}_KERNEL_SIZE_X
#define {NAME}_WR_KERNEL_SIZE_Y   {WR_LAYER}_KERNEL_SIZE_Y

#define {NAME}_SIZE_WR  DIVIDE({NAME}_WR_WEIGHTS,{NAME}_STREAMS_WR)

#define {NAME}_WR_BATCH_SIZE    1
#define {NAME}_WR_ROWS_IN       1
#define {NAME}_WR_COLS_IN       1
#define {NAME}_WR_CHANNELS_IN   {NAME}_SIZE_WR
#define {NAME}_WR_PORTS_IN      {NAME}_PORTS_WR
#define {NAME}_WR_STREAMS_IN    {NAME}_STREAMS_WR

void reload_weights(
    int weights_reloading_index,
    volatile mem_int wr_hw[{NAME}_PORTS_WR][{NAME}_SIZE_WR],
    {wr_layer}_weight_t weights[{NAME}_WR_COARSE_IN*{NAME}_WR_COARSE_GROUP][{NAME}_WR_COARSE_OUT][DIVIDE({NAME}_WR_WEIGHTS,{NAME}_WR_COARSE_IN*{NAME}_WR_COARSE_GROUP*{NAME}_WR_COARSE_OUT*{NAME}_WR_KERNEL_SIZE_X*{NAME}_WR_KERNEL_SIZE_Y)][{NAME}_WR_KERNEL_SIZE_X][{NAME}_WR_KERNEL_SIZE_Y]
);
#endif

#define {NAME}_DMA_WIDTH            {DMA_WIDTH}
#define {NAME}_IN_DATA_WIDTH        {input_data_width}
#define {NAME}_WEIGHTS_DATA_WIDTH   {weight_data_width}
#define {NAME}_OUT_DATA_WIDTH       {output_data_width}

void process(
    int weights_reloading_index,
    volatile mem_int in_hw[{NAME}_PORTS_IN][{NAME}_SIZE_IN],
    volatile mem_int out_hw[{NAME}_PORTS_OUT][{NAME}_SIZE_OUT]
);

void fpgaconvnet_ip(
    int mode,
    int weights_reloading_index,
#if {NAME}_WEIGHTS_RELOADING_FLAG
    volatile mem_int wr_hw[{NAME}_PORTS_WR][{NAME}_SIZE_WR],
#endif
    volatile mem_int in_hw[{NAME}_PORTS_IN][{NAME}_SIZE_IN],
    volatile mem_int out_hw[{NAME}_PORTS_OUT][{NAME}_SIZE_OUT]
);

#endif
"""

network_src_template = """#include "{name}_top.hpp"

{weights}

{biases}

#if {NAME}_WEIGHTS_RELOADING_FLAG
void reload_weights(
    int weights_reloading_index,
    volatile mem_int wr_hw[{NAME}_PORTS_WR][{NAME}_SIZE_WR],
    {wr_layer}_weight_t weights[{NAME}_WR_COARSE_IN*{NAME}_WR_COARSE_GROUP][{NAME}_WR_COARSE_OUT][DIVIDE({NAME}_WR_WEIGHTS,{NAME}_WR_COARSE_IN*{NAME}_WR_COARSE_GROUP*{NAME}_WR_COARSE_OUT*{NAME}_WR_KERNEL_SIZE_X*{NAME}_WR_KERNEL_SIZE_Y)][{NAME}_WR_KERNEL_SIZE_X][{NAME}_WR_KERNEL_SIZE_Y]
)
{{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

#pragma HLS stable variable=weights

    // stream init
    stream_t({wr_layer}_weight_t) wr[{NAME}_STREAMS_WR];
#pragma HLS STREAM variable=wr
#pragma HLS ARRAY_PARTITION variable=wr complete dim=0

    mem_read<
        {NAME}_WR_BATCH_SIZE,
        {NAME}_WR_ROWS_IN,
        {NAME}_WR_COLS_IN,
        {NAME}_WR_CHANNELS_IN,
        {NAME}_WR_PORTS_IN,
        {NAME}_WR_STREAMS_IN,
        {wr_layer}_weight_t,
        {NAME}_DMA_WIDTH,
        {NAME}_WEIGHTS_DATA_WIDTH
    >(wr_hw,wr);

    weights_reloading<
       {NAME}_WR_WEIGHTS,
       {NAME}_WR_COARSE_IN,
       {NAME}_WR_COARSE_OUT,
       {NAME}_WR_COARSE_GROUP,
       {NAME}_WR_KERNEL_SIZE_X,
       {NAME}_WR_KERNEL_SIZE_Y,
       {wr_layer}_weight_t
    >(wr[0],weights);
}}
#endif

void process(
    int weights_reloading_index,
    volatile mem_int in_hw[{NAME}_PORTS_IN][{NAME}_SIZE_IN],
    volatile mem_int out_hw[{NAME}_PORTS_OUT][{NAME}_SIZE_OUT]
)
{{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

{weights_init}
{biases_init}
{streams_init}

    mem_read<
        {NAME}_BATCH_SIZE,
        {NAME}_ROWS_IN,
        {NAME}_COLS_IN,
        {NAME}_CHANNELS_IN,
        {NAME}_PORTS_IN,
        {NAME}_STREAMS_IN,
        {name}_input_t,
        {NAME}_DMA_WIDTH,
        {NAME}_IN_DATA_WIDTH
    >(in_hw,in);

    int mode = 0;

{layers}

    mem_write<
        {NAME}_BATCH_SIZE,
        {NAME}_ROWS_OUT,
        {NAME}_COLS_OUT,
        {NAME}_CHANNELS_OUT,
        {NAME}_PORTS_OUT,
        {NAME}_STREAMS_OUT,
        {NAME}_WEIGHTS_RELOADING_FACTOR,
        {name}_output_t,
        {NAME}_DMA_WIDTH,
        {NAME}_OUT_DATA_WIDTH
    >(weights_reloading_index,out,out_hw);

}}

void fpgaconvnet_ip(
    int mode,
    int weights_reloading_index,
#if {NAME}_WEIGHTS_RELOADING_FLAG
    volatile mem_int wr_hw[{NAME}_PORTS_WR][{NAME}_SIZE_WR],
#endif
    volatile mem_int in_hw[{NAME}_PORTS_IN][{NAME}_SIZE_IN],
    volatile mem_int out_hw[{NAME}_PORTS_OUT][{NAME}_SIZE_OUT]
)
{{
#pragma HLS INTERFACE s_axilite port=return                     bundle=ctrl
#pragma HLS INTERFACE s_axilite port=mode                       bundle=ctrl
#pragma HLS INTERFACE s_axilite port=weights_reloading_index    bundle=ctrl

#if {NAME}_WEIGHTS_RELOADING_FLAG
#pragma HLS ARRAY_PARTITION variable=wr_hw  complete dim=1
#endif
#pragma HLS ARRAY_PARTITION variable=in_hw  complete dim=1
#pragma HLS ARRAY_PARTITION variable=out_hw complete dim=1

#if {NAME}_WEIGHTS_RELOADING_FLAG
    const unsigned size_wr  = {NAME}_SIZE_WR ;
#endif
    const unsigned size_in  = {NAME}_SIZE_IN ;
    const unsigned size_out = {NAME}_SIZE_OUT;

#if {NAME}_WEIGHTS_RELOADING_FLAG
#pragma HLS INTERFACE m_axi port=wr_hw  offset=slave depth=size_wr  num_read_outstanding=1 num_write_outstanding=1 max_read_burst_length=256 max_write_burst_length=256 name=fpgaconvnet_wr  bundle=fpgaconvnet_port_wr
#endif

#pragma HLS INTERFACE m_axi port=in_hw  offset=slave depth=size_in  num_read_outstanding=1 num_write_outstanding=1 max_read_burst_length=256 max_write_burst_length=256 name=fpgaconvnet_in  bundle=fpgaconvnet_port_in

#pragma HLS INTERFACE m_axi port=out_hw offset=slave depth=size_out num_read_outstanding=1 num_write_outstanding=1 max_read_burst_length=256 max_write_burst_length=256 name=fpgaconvnet_out bundle=fpgaconvnet_port_out


    #pragma HLS DATAFLOW
    if ( mode == 0 ) {{
        process(weights_reloading_index,in_hw,out_hw);
    }} else if ( mode == 1 ) {{
#if {NAME}_WEIGHTS_RELOADING_FLAG
        reload_weights(weights_reloading_index,wr_hw,{wr_layer}_weights);
#endif
    }}

}}
"""

network_tb_src_template = """#include "{name}_top.hpp"
#include "common_tb.hpp"

int main()
{{
    int err = 0;

    static mem_int test_in[{NAME}_PORTS_IN][{NAME}_SIZE_IN] = {{0}};

    // load input
    printf("LOADING INPUT DATA \\n");
    load_net_data<
        {NAME}_PORTS_IN,
        {NAME}_BATCH_SIZE,
        {NAME}_ROWS_IN,
        {NAME}_COLS_IN,
        {NAME}_CHANNELS_IN,
        {NAME}_STREAMS_IN
    >("{input_data_path}",test_in);

    for( int wr_index=0;wr_index<{NAME}_WEIGHTS_RELOADING_FACTOR;wr_index++) {{

        static mem_int test_out[{NAME}_PORTS_OUT][{NAME}_SIZE_OUT]          = {{0}};
        static mem_int test_out_valid[{NAME}_PORTS_OUT][{NAME}_SIZE_OUT]    = {{0}};

#if {NAME}_WEIGHTS_RELOADING_FLAG
        static mem_int weights[{NAME}_PORTS_WR][{NAME}_SIZE_WR] = {{0}};

        // load weights
        printf("LOADING WEIGHTS \\n");
        load_net_weights<
            {NAME}_PORTS_WR,
            {NAME}_SIZE_WR,
            {NAME}_WEIGHTS_RELOADING_FACTOR
        >("{weights_reloading_path}", weights, wr_index);
#endif

        // load valid output
        printf("LOADING VALID OUTPUT DATA \\n");
        load_net_data<
            {NAME}_PORTS_OUT,
            {NAME}_BATCH_SIZE,
            {NAME}_ROWS_OUT,
            {NAME}_COLS_OUT,
            {NAME}_CHANNELS_OUT,
            {NAME}_STREAMS_OUT,
            {NAME}_WEIGHTS_RELOADING_FACTOR
        >("{output_data_path}", test_out_valid, wr_index);

        printf("RUNNING NETWORK \\n");

        // perform weights reloading
#if {NAME}_WEIGHTS_RELOADING_FLAG
        if( wr_index > 0 ) {{
            fpgaconvnet_ip(1,wr_index,weights,test_in,test_out);
        }}
#endif   

        // run the network
#if {NAME}_WEIGHTS_RELOADING_FLAG
        fpgaconvnet_ip(0,wr_index,weights,test_in,test_out);
#else
        fpgaconvnet_ip(0,wr_index,test_in,test_out);
#endif

        // check array is correct
        for(int i=0; i<{NAME}_PORTS_OUT;i++) {{
            printf("PORT %d\\n",i);
            err += check_array_equal<{NAME}_SIZE_OUT, {NAME}_STREAMS_OUT>(test_out[i],test_out_valid[i]);
        }}

    }}

    printf("%s\\n",(err==0) ? "\\t--- PASSED ---" : "\\t--- FAILED ---");
    return err;
}}
"""
