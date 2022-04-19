import json
import os
import shutil
import numpy as np
import sys
from functools import reduce
from pathlib import Path
import subprocess

from fpgaconvnet.hls.generate.partition_template import *

from fpgaconvnet.hls.generate.layers.convolution    import gen_convolution_layer
from fpgaconvnet.hls.generate.layers.pooling        import gen_pooling_layer
from fpgaconvnet.hls.generate.layers.relu           import gen_relu_layer
from fpgaconvnet.hls.generate.layers.inner_product  import gen_inner_product_layer
from fpgaconvnet.hls.generate.layers.squeeze        import gen_squeeze_layer
from fpgaconvnet.hls.generate.util import *

import fpgaconvnet.tools.graphs as graphs
import fpgaconvnet.proto.fpgaconvnet_pb2 as fpgaconvnet_pb2
from google.protobuf.json_format import MessageToDict
from fpgaconvnet.hls.tools.onnx_data import get_layer_from_partition, gen_layer_name # REQUIRED EDIT

class GeneratePartition:

    def __init__(self, name, partition, onnx_data, output_path):

        self.name = name
        self.partition = partition
        self.output_path = output_path
        self.onnx_data = onnx_data

        # make output path directory
        self.mkdir(self.output_path)
        self.mkdir(os.path.join(self.output_path, "src"))
        self.mkdir(os.path.join(self.output_path, "tb"))
        self.mkdir(os.path.join(self.output_path, "include"))
        self.mkdir(os.path.join(self.output_path, "data"))

        # get fpgaconvnet-hls root directory
        self.fpgaconvnet_root = str(Path(__file__).resolve().parent.parent)

        # state of generated partition
        self.is_generated = {
            "layers" : False,
            "weights" : False,
            "streams" : False,
            "source" : False,
            "include" : False,
            "testbench" : False
        }

    def mkdir(self, path):
        try:
            os.mkdir(path)
        except FileExistsError:
            print(f"WARNING: path {path} already exists!")

    def generate_layers(self):

        self.layers = ""

        # generate hardware
        for layer in self.partition.layers:
            # get parameters of layer
            parameters = MessageToDict(layer.parameters, preserving_proto_field_name=True)
            # init function arguments for this layer
            fn_args = []
            layer_name = gen_layer_name(layer)
            # init hardware generation args
            args = [
                layer_name,
                parameters,
                os.path.join(self.output_path, "src", f"{layer_name}.cpp"),
                os.path.join(self.output_path, "include", f"{layer_name}.hpp")
            ]
            # create hardware for each layer
            if layer.type == fpgaconvnet_pb2.layer.layer_type.CONVOLUTION:
                fn_args.append(f"{layer_name}_weights")
                gen_convolution_layer(*args)
            if layer.type == fpgaconvnet_pb2.layer.layer_type.POOLING:
                gen_pooling_layer(*args)
            if layer.type == fpgaconvnet_pb2.layer.layer_type.CONCAT:
                gen_concat_layer(*args)
            if layer.type == fpgaconvnet_pb2.layer.layer_type.RELU:
                gen_relu_layer(*args)
            if layer.type == fpgaconvnet_pb2.layer.layer_type.SPLIT:
                gen_split_layer(*args)
            if layer.type == fpgaconvnet_pb2.layer.layer_type.INNER_PRODUCT:
                fn_args.append(f"{layer_name}_weights")
                gen_inner_product_layer(*args)
            if layer.type == fpgaconvnet_pb2.layer.layer_type.SQUEEZE:
                gen_squeeze_layer(*args)
            # create layer call
            for stream_in in layer.streams_in:
                fn_args.append(stream_in.name)
            for stream_out in layer.streams_out:
                fn_args.append(stream_out.name)
            fn_args.append("mode")
            fn_args = ", ".join(fn_args)
            self.layers += f"    {layer_name}({fn_args});\n"

        # set generated flag
        self.is_generated["layers"] = True

    def generate_weights(self):

        weights = []

        # generate weights
        for layer in self.partition.layers:
            # get parameters of layer
            parameters = MessageToDict(layer.parameters, preserving_proto_field_name=True)
            # get layer name
            layer_name = gen_layer_name(layer)
            # create hardware for each layer
            if layer.type in [fpgaconvnet_pb2.layer.layer_type.CONVOLUTION, fpgaconvnet_pb2.layer.layer_type.INNER_PRODUCT]:
                # add weight generators
                weights.append(GenerateWeights(layer_name,
                        wr=(layer_name == self.partition.weights_reloading_layer)))
                # create weights from onnx model
                ## get weights reloading factor
                wr_factor = self.partition.weights_reloading_factor \
                        if layer_name == self.partition.weights_reloading_layer else 1
                ## save both dat and csv forms of weights
                self.onnx_data.save_weights_layer(layer, wr_factor=wr_factor,
                       output_path=os.path.join(self.output_path, "data", f"{layer_name}_weights"),
                       to_dat=True, to_csv=True)
                ## also save biases to dat and csv
                # self.onnx_data.save_biases_layer(layer,
                #        output_path=os.path.join(self.output_path, "data", f"{layer_name}_biases"),
                #        to_dat=True, to_csv=True)

        # get weights definitions and intialisation
        self.weights_def = "\n\n".join([w.generate_def() for w in weights])
        self.weights_init = "\n\n".join([w.generate_init() for w in weights])

        # set generated flag
        self.is_generated["weights"] = True

    def generate_streams(self):
        # get all streams
        streams = {}
        for layer in self.partition.layers:
            for stream_in in layer.streams_in:
                streams[stream_in.name] = GenerateStreams(stream_in.name, f"{layer.name}_input_t",
                        [f"{layer.name.upper()}_COARSE_IN"])
            for stream_out in layer.streams_out:
                streams[stream_out.name] = GenerateStreams(stream_out.name, f"{layer.name}_output_t",
                        [f"{layer.name.upper()}_COARSE_OUT"])

        # create stream initialisations
        self.streams_init = "\n".join([s.generate_stream() for s in streams.values()])

        # set generated flag
        self.is_generated["streams"] = True

    def generate_include(self):
        # include generation
        include = ""
        for layer in self.partition.layers:
            layer_name = gen_layer_name(layer)
            include +=f"#include \"{layer_name}.hpp\"\n"

        # HEADER
        network_header = network_header_template.format(
            name        =self.name,
            NAME        =self.name.upper(),
            batch_size  =self.partition.batch_size,
            rows_in     =self.partition.layers[0].parameters.rows_in,
            cols_in     =self.partition.layers[0].parameters.cols_in,
            channels_in =self.partition.layers[0].parameters.channels_in,
            rows_out    =self.partition.layers[-1].parameters.rows_out,
            cols_out    =self.partition.layers[-1].parameters.cols_out,
            channels_out=self.partition.layers[-1].parameters.channels_out,
            ports       =self.partition.ports,
            streams_in  =self.partition.layers[0].parameters.coarse_in,
            streams_out =self.partition.layers[-1].parameters.coarse_out,
            input_layer =self.partition.layers[0].name,
            output_layer=self.partition.layers[-1].name,
            wr_layer    =self.partition.weights_reloading_layer,
            WR_LAYER    =self.partition.weights_reloading_layer.upper(),
            wr_factor   =self.partition.weights_reloading_factor,
            wr_flag     =int(self.partition.weights_reloading_layer != "None"),
            include     =include
        )

        # save to output path
        with open(os.path.join(self.output_path, f'include/{self.name}_top.hpp'),'w') as f:
            f.write(network_header)

        # set generated flag
        self.is_generated["include"] = True

    def generate_source(self):

        # todo: check for layers, weights, streams
        assert self.is_generated["layers"], "ERROR: layers not generated!"
        assert self.is_generated["weights"], "ERROR: weights not generated!"
        assert self.is_generated["streams"], "ERROR: weights not generated!"

        # format the source code template
        network_src = network_src_template.format(
            name        =self.name,
            NAME        =self.name.upper(),
            wr_layer    =self.partition.weights_reloading_layer,
            weights     =self.weights_def,
            weights_init=self.weights_init,
            streams_init=self.streams_init,
            layers      =self.layers
        )

        # save to output path
        with open(os.path.join(self.output_path, f'src/{self.name}_top.cpp'),'w') as f:
            f.write(network_src)

        # set generated flag
        self.is_generated["source"] = True

    def generate_testbench(self):

        # generate the data in and out of the partition

        # format testbench code template
        network_tb_src = network_tb_src_template.format(
            name = self.name,
            NAME = self.name.upper(),
            input_data_path = os.path.join(self.output_path, f"data/{self.partition.layers[0].name}_0.dat"),
            weights_reloading_path = os.path.join(self.output_path,
                f"data/{self.partition.weights_reloading_layer}_weights_0.dat"),
            output_data_path = os.path.join(self.output_path, f"data/{self.partition.layers[0].name}_0.dat")
        )

        # save to output path
        with open(os.path.join(self.output_path,f'tb/{self.name}_tb.cpp'),'w') as f:
            f.write(network_tb_src)

        # set generated flag
        self.is_generated["testbench"] = True

    """
    Vivado HLS functions
    """

    def create_vivado_hls_project(self, fpga_part="xc7z045ffg900-2", clk=5):

        # check everything is generated
        assert reduce(lambda a, b: a & b, self.is_generated.values()), "ERROR: not all stages are generated!"

        # create hls project
        os.system(f"vivado_hls -f {self.fpgaconvnet_root}/scripts/hls/create_partition_project.tcl\
                \"_ -name {self.name} -prj {self.output_path} -fpga {fpga_part} -clk {clk}\"")

    def run_csynth(self):

        # create hls project
        os.system(f"vivado_hls -f {self.fpgaconvnet_root}/scripts/hls/run_csynth.tcl\
                \"_ -name {self.name}\"")


if __name__=="__main__":

    gen_network(
        'lenet_test',
        'test/networks/lenet_test/lenet_test_partition_info.json',
        'test/networks/lenet_test')
