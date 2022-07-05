import onnx
import onnxruntime
from google.protobuf import json_format
import numpy as np

import fpgaconvnet.proto.fpgaconvnet_pb2
import fpgaconvnet.tools.onnx_helper as onnx_helper

from fpgaconvnet.hls.generate.partition import GeneratePartition

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

    def __init__(self, name, partition_path, model_path, fpga_part="xc7z045ffg900-2", clk=5):

        # save name
        self.name = name

        # save platform information
        self.fpga_part = fpga_part
        self.clk = clk

        # load partition information
        self.partitions = fpgaconvnet.proto.fpgaconvnet_pb2.partitions()
        with open(partition_path,'r') as f:
           json_format.Parse(f.read(), self.partitions)

        # load onnx model
        self.model = onnx_helper.load(model_path)
        self.model = onnx_helper.update_batch_size(self.model, 1) # TODO
        # self.model = onnx_helper.update_batch_size(self.model,self.partition.batch_size)

        # remove biases
        for partition in self.partitions.partition:
            for layer in partition.layers:
                if layer.bias_path:
                    initializer = onnx_helper.get_model_initializer(self.model, layer.bias_path, to_tensor=False)
                    # TODO: seems like theres no bias initializer for inner product layer
                    if not initializer:
                        continue
                    zeroes = np.zeros(onnx.numpy_helper.to_array(initializer).shape).astype(np.float32)
                    initializer_new = onnx.numpy_helper.from_array(zeroes,name=initializer.name)
                    self.model.graph.initializer.remove(initializer)
                    self.model.graph.initializer.extend([initializer_new])

        # add intermediate layers to outputs
        for node in self.model.graph.node:
            layer_info = onnx.helper.ValueInfoProto()
            layer_info.name = node.output[0]
            self.model.graph.output.append(layer_info)

        # add input aswell to output
        layer_info = onnx.helper.ValueInfoProto()
        layer_info.name = self.model.graph.input[0].name
        self.model.graph.output.append(layer_info)

        # remove input initializers
        name_to_input = {}
        inputs = self.model.graph.input
        for input in inputs:
            name_to_input[input.name] = input
        for initializer in self.model.graph.initializer:
            if initializer.name in name_to_input:
                inputs.remove(name_to_input[initializer.name])

        # inference session
        self.sess = onnxruntime.InferenceSession(self.model.SerializeToString())

        # create generator for each partition
        self.partitions_generator = [ GeneratePartition(
            self.name, partition, self.model, self.sess, f"partition_{i}") for \
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


