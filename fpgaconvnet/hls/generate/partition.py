import json
import os
import shutil
import numpy as np
import sys

sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

from generate.partition_template import *

from generate.layers.convolution    import gen_convolution_layer
from generate.layers.pooling        import gen_pooling_layer
#from generate.layers.concat         import gen_concat_layer
from generate.layers.relu           import gen_relu_layer
#from generate.layers.split          import gen_split_layer
from generate.layers.inner_product  import gen_inner_product_layer
from generate.layers.squeeze        import gen_squeeze_layer

import fpgaconvnet_optimiser.tools.graphs as graphs
import fpgaconvnet_optimiser.proto.fpgaconvnet_pb2 as fpgaconvnet_pb2
from google.protobuf.json_format import MessageToDict
from tools.onnx_data import get_layer_from_partition, gen_layer_name # REQUIRED EDIT

# class for weight definition
class generate_weight_def:
    # initialise class
    def __init__(self, name, kernel_size_x=1, kernel_size_y=1, wr=False):
        self.name       = name
        self.type       = "static" if wr else "const static"
        self.kernel_dim = "[{NAME}_KERNEL_SIZE_X][{NAME}_KERNEL_SIZE_Y]".format(NAME=self.name.upper())

    def __str__(self):
        return """
{type} {name}_weight_t {name}_weights[{NAME}_COARSE_IN*{NAME}_COARSE_GROUP][{NAME}_COARSE_OUT][DIVIDE({NAME}_WEIGHTS,{NAME}_COARSE_IN*{NAME}_COARSE_GROUP*{NAME}_COARSE_OUT*{NAME}_KERNEL_SIZE_X*{NAME}_KERNEL_SIZE_Y)]{kernel_dim} = {{
#include "{name}_weights_0.csv"
}};
""".format(
        NAME=self.name.upper(),
        name=self.name,
        type=self.type,
        kernel_dim=self.kernel_dim
    )

# class for weight initialisation
class generate_weight_init():
    # initialise class
    def __init__(self, name, wr=False):
        self.name = name
        self.bram_type = "RAM" if wr else "ROM"

    def __str__(self):
        return """
#pragma HLS ARRAY_PARTITION variable={name}_weights complete dim=1
#pragma HLS ARRAY_PARTITION variable={name}_weights complete dim=2
#pragma HLS RESOURCE variable={name}_weights core={bram_type}
//#pragma HLS STREAM variable={name}_weights off
#pragma HLS STABLE variable={name}_weights

""".format(name=self.name,bram_type=self.bram_type)

## class for stream initialisation
#class generate_stream_init():
#    # initialise class
#    def __init__(self):
#
#    def __str__(self):
#        return ''
#
#    def __repr__(self):
#        return self.__str__()

def gen_network(name,partition,output_path):

    def _fix_identifier(name):
        if name[0].isdigit():
            return "n" + name
        else:
            return name

    partition.weights_reloading_layer = _fix_identifier(partition.weights_reloading_layer)
    for layer in partition.layers:
        for stream_in in layer.streams_in:
            stream_in.name = _fix_identifier(stream_in.name)
        for stream_out in layer.streams_out:
            stream_out.name = _fix_identifier(stream_out.name)

        layer.name = _fix_identifier(layer.name)


    wr_layer = partition.weights_reloading_layer
    wr_layer_identifier = _fix_identifier(wr_layer).replace("/", "_")

    batch_size = partition.batch_size

    input_node  = partition.input_node
    output_node = partition.output_node

    # get all streams
    streams = {}
    for layer in partition.layers:
        for stream_in in layer.streams_in:
            streams[stream_in.name] = (stream_in.coarse, f"{layer.name}_input_t")
        for stream_out in layer.streams_out:
            streams[stream_out.name] = (stream_out.coarse, f"{layer.name}_output_t")

    # remove duplicates
    # streams = list(set(streams))

    # create stream initialisations
    streams_init = ""
    for stream in streams:
        streams_init +=  """
    stream_t({stream_type}) {stream_name}[{coarse}];
#pragma HLS STREAM variable={stream_name}
#pragma HLS ARRAY_PARTITION variable={stream_name} complete dim=0
""".format(stream_name=stream,coarse=streams[stream][0],stream_type=streams[stream][1])

    # weight information
    weights = ""
    weights_init = ""

    # layers initialisation
    layers = ""

    # generate hardware
    for layer in partition.layers:
        # get parameters of layer
        parameters = MessageToDict(layer.parameters, preserving_proto_field_name=True)
        # init function arguments for this layer
        fn_args=[]
        layer_name = gen_layer_name(layer)
        # init hardware generation args
        args = [
            layer_name,
            parameters,
            os.path.join(output_path,'src',f'{layer_name}.cpp'),
            os.path.join(output_path,'include',f'{layer_name}.hpp')
        ]
        if layer.type == fpgaconvnet_pb2.layer.layer_type.CONVOLUTION:
            # add weights to function arguments
            fn_args.append(f"{layer_name}_weights")
            # generate hardware
            gen_convolution_layer(*args)
            # create weights
            weights += str(generate_weight_def(
                layer_name,
                kernel_size_x=int(parameters["kernel_size"][0]),
                kernel_size_y=int(parameters["kernel_size"][1]),
                wr=True if layer_name == wr_layer_identifier else False
            ))
            weights_init += str(generate_weight_init(
                layer_name,
                wr=True if layer_name == wr_layer_identifier else False
            ))
        if layer.type == fpgaconvnet_pb2.layer.layer_type.POOLING:
            gen_pooling_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.CONCAT:
            gen_concat_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.RELU:
            gen_relu_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.SPLIT:
            gen_split_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.INNER_PRODUCT:
            # add weights to function arguments
            fn_args.append(f"{layer_name}_weights")
            # generate hardware
            gen_inner_product_layer(*args)
            # create weights
            weights += str(generate_weight_def(
                layer_name,
                kernel_size_x=1,
                kernel_size_y=1,
                wr=True if layer_name == wr_layer_identifier else False
            ))
            weights_init += str(generate_weight_init(
                layer_name,
                wr=True if layer_name == wr_layer_identifier else False
            ))
        if layer.type == fpgaconvnet_pb2.layer.layer_type.SQUEEZE:
            gen_squeeze_layer(*args)
        # add layer function
        for stream_in in layer.streams_in:
            fn_args.append(stream_in.name)
        for stream_out in layer.streams_out:
            fn_args.append(stream_out.name)
        fn_args.append("mode")
        fn_args = ", ".join(fn_args)
        layers += f"    {layer_name}({fn_args});\n"

    # include generation
    include = ""
    for layer in partition.layers:
        layer_name = gen_layer_name(layer)
        include +=f"#include \"{layer_name}.hpp\"\n"

    # HEADER
    network_header = network_header_template.format(
        name        =name,
        NAME        =name.upper(),
        batch_size  =batch_size,
        rows_in     =partition.layers[0].parameters.rows_in,
        cols_in     =partition.layers[0].parameters.cols_in,
        channels_in =partition.layers[0].parameters.channels_in,
        rows_out    =partition.layers[-1].parameters.rows_out,
        cols_out    =partition.layers[-1].parameters.cols_out,
        channels_out=partition.layers[-1].parameters.channels_out,
        ports       =partition.ports,
        streams_in  =partition.layers[0].parameters.coarse_in, # TODO: change
        streams_out =partition.layers[-1].parameters.coarse_out, # TODO: change
        input_layer =partition.layers[0].name,
        output_layer=partition.layers[-1].name,
        wr_layer    =wr_layer_identifier,
        WR_LAYER    =wr_layer_identifier.upper(),
        wr_factor   =partition.weights_reloading_factor,
        wr_flag     =int(wr_layer != "None"),
        include     =include
    )
    # SRC
    network_src = network_src_template.format(
        name        =name,
        NAME        =name.upper(),
        wr_layer    =wr_layer_identifier,
        weights     =weights,
        weights_init=weights_init,
        streams_init=streams_init,
        layers      =layers
    )
    # TB
    network_tb_src = network_tb_src_template.format(
        name = name,
        NAME = name.upper(),
        input_data_path = os.path.join(os.getcwd(), output_path, f"data/{input_node}_0.dat"),
        weights_reloading_path = os.path.join(os.getcwd(), output_path, f"data/{wr_layer}_weights_0.dat"),
        output_data_path = os.path.join(os.getcwd(), output_path, f"data/{output_node}_0.dat")
    )

    with open(os.path.join(output_path,f'include/{name}_top.hpp'),'w') as f:
        f.write(network_header)
    with open(os.path.join(output_path,f'src/{name}_top.cpp'),'w') as f:
        f.write(network_src)
    with open(os.path.join(output_path,f'tb/{name}_tb.cpp'),'w') as f:
        f.write(network_tb_src)

if __name__=="__main__":

    gen_network(
        'lenet_test',
        'test/networks/lenet_test/lenet_test_partition_info.json',
        'test/networks/lenet_test')
