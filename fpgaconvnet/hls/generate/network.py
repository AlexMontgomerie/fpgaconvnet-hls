import onnx
import onnxruntime
from google.protobuf import json_format
import numpy as np

import fpgaconvnet.proto.fpgaconvnet_pb2
from fpgaconvnet.parser.Parser import Parser
import fpgaconvnet.parser.onnx.helper as onnx_helper

from fpgaconvnet.hls.generate.partition import GeneratePartition
from fpgaconvnet.hls.generate.network_template import *
from pathlib import Path

class GenerateNetwork:
    """
    Base class for all layer models.

    Attributes
    ----------
    buffer_depth: int, default: 0
        depth of incoming fifo buffers for each stream in.
    model:
        an onnx model loaded from the given `model_path`
    partitions_generator: List[GeneratePartition]
        list of partition generators for each partition loaded
        from the `partition_path`
    """

    def __init__(self, name, partition_path, model_path, fpga_part="xc7z045ffg900-2", clk=5, port_width=64):

        # save name
        self.name = name

        # save platform information
        self.fpga_part = fpga_part
        self.clk = clk
        
        # save port_width
        self.port_width = port_width
        
        # configure data types
        if self.port_width == 8: 
            write_data_type = "ap_uint<8>"
            write_ap_axis_param = "8,1,1,1"
        elif self.port_width == 16:
            write_data_type = "ap_uint<16>"
            write_ap_axis_param = "16,1,1,1"
        elif self.port_width == 32:
            write_data_type = "ap_uint<32>"
            write_ap_axis_param = "32,1,1,1"
        elif self.port_width == 64:
            write_data_type = "ap_uint<64>"
            write_ap_axis_param = "64,1,1,1"
        elif self.port_width == 128:
            write_data_type = "ap_uint<128>"
            write_ap_axis_param = "128,1,1,1"
        else:
            raise ValueError("port_width must be 8, 16, 32, 64, or 128")
        
        # write common.hpp 
        write_common = config_common.format(
            DATA_TYPE = write_data_type, 
            AP_AXIS_PARAM = write_ap_axis_param
        )
        with open (Path(__file__).parent.parent /"hardware"/"common.hpp", "w") as f:
            f.write(write_common)
        
        # write common_tb.hpp
        write_common_tb = config_common_tb.format(
            DMA_WIDTH = self.port_width
        )
        with open (Path(__file__).parent.parent /"hardware"/"common_tb.hpp", "w") as f:
            f.write(write_common_tb)

        # load partition information
        self.partitions = fpgaconvnet.proto.fpgaconvnet_pb2.partitions()
        with open(partition_path,'r') as f:
           json_format.Parse(f.read(), self.partitions)

        # set up parser
        print("FIXME: use HLS backend in Parser")
        self.parser = Parser(backend="chisel", batch_size=self.partitions.partition[0].batch_size) # FIXME for hls backend, resource model is missing
        # modify network to be supported by onnx
        model = onnx.load(model_path)
        nodes = model.graph.node
        for node in nodes:
            attrs = node.attribute
            for attr in attrs[:]:
                if attr.name == 'weight_width': 
                    assert attr.i == 16, "weight_width must be 16"
                    attrs.remove(attr)
                elif attr.name == 'data_width':
                    assert attr.i == 16, "data_width must be 16"
                    attrs.remove(attr)
                elif attr.name == 'acc_width':
                    attrs.remove(attr)
                elif attr.name == 'block_floating_point':
                    assert attr.s == (b''), "block_floating_point must be false"
                    attrs.remove(attr)
        new_model_path = model_path.replace('.onnx','_new.onnx')
        onnx.save(model, new_model_path)
        # load network (parser onnx model)
        self.net = self.parser.onnx_to_fpgaconvnet(new_model_path)
        # load the existing partition information into the net object
        self.net = self.parser.prototxt_to_fpgaconvnet(self.net,partition_path)

        # add intermediate layers to outputs
        for node in self.net.model.graph.node:
            layer_info = onnx.helper.ValueInfoProto()
            layer_info.name = node.output[0]
            self.net.model.graph.output.append(layer_info)

        # add input aswell to outpuexisting t
        layer_info = onnx.helper.ValueInfoProto()
        layer_info.name = self.net.model.graph.input[0].name
        self.net.model.graph.output.append(layer_info)

        # remove input initializers
        name_to_input = {}
        inputs = self.net.model.graph.input
        for input in inputs:
            name_to_input[input.name] = input
        for initializer in self.net.model.graph.initializer:
            if initializer.name in name_to_input:
                inputs.remove(name_to_input[initializer.name])

        # inference session
        self.sess = onnxruntime.InferenceSession(self.net.model.SerializeToString())

        # create generator for each partition
        self.partitions_generator = [ GeneratePartition(
            self.name, partition, self.net.model, self.sess, f"partition_{i}", self.port_width) for \
                    i, partition in enumerate(self.partitions.partition) ]

        # flags
        self.is_generated = {
            "project" : False,
            "hardware" : False
        }

    def create_partition_project(self, partition_index, reset=False):
        # generate each part of the partition
        self.partitions_generator[partition_index].generate_layers()
        self.partitions_generator[partition_index].generate_parameters()
        self.partitions_generator[partition_index].generate_streams()
        self.partitions_generator[partition_index].generate_include()
        self.partitions_generator[partition_index].generate_source()
        self.partitions_generator[partition_index].generate_testbench()

        # create HLS project
        self.partitions_generator[partition_index].create_vivado_hls_project(
                fpga_part=self.fpga_part, clk=self.clk)

        # set project generated flag
        self.is_generated["project"] = True

    def generate_partition_hardware(self, partition_index):
        """
        Generates the hardware for the given parititon in the network.
        Creates the HLS project, runs HLS synthesis and then packages the
        generated IP.

        Parameters
        ----------
        partition_index: int
        """

        if not self.is_generated["project"]:
            print("WARNING: partition project not created! creating now ...")
            self.create_partition_project(partition_index)

        # run c-synthesis
        self.partitions_generator[partition_index].run_csynth()

        # export IP package
        self.partitions_generator[partition_index].export_design()

        # set hardware generation flag
        self.is_generated["hardware"] = True

    def run_testbench(self, partition_index, image=None):
        """
        Generates the hardware for the given parititon in the network.
        Creates the HLS project, runs HLS synthesis and then packages the
        generated IP.

        Parameters
        ----------
        partition_index: int
        """

        if not self.is_generated["project"]:
            print("WARNING: partition project not created! creating now ...")
            self.create_partition_project(partition_index)

        if image is not None:
            # create the testbench data
            self.partitions_generator[partition_index].create_testbench_data(image)

        # run the c-simulation
        self.partitions_generator[partition_index].run_csim()

    def run_cosimulation(self, partition_index, image=None):
        """
        Generates the hardware for the given parititon in the network.
        Creates the HLS project, runs HLS synthesis and then packages the
        generated IP.

        Parameters
        ----------
        partition_index: int
        """

        if not self.is_generated["project"]:
            print("WARNING: partition project not created! creating now ...")
            self.create_partition_project(partition_index)

        # if not self.is_generated["hardware"]:
        #     print("WARNING: partition has not been generated! generating now ...")
        #     self.generate_partition_hardware(partition_index)

        if image is not None:
            # create the testbench data
            self.partitions_generator[partition_index].create_testbench_data(image)

        # run the c-simulation
        self.partitions_generator[partition_index].run_cosim()

    def generate_all_partitions(self, num_jobs=1):
        """
        Runs `generate_partition` for all partitions in the network.

        Parameters
        ----------
        num_jobs: int = 0
            number of parallel jobs to execute for partition generation
            .. note::
                no parallel execution implemented yet
        """

        # TODO: add multi-threading for partitions
        for i in range(len(self.partitions_generator)):
            self.generate_partition(i)

