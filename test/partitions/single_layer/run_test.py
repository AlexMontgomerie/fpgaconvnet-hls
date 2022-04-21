from google.protobuf import json_format

import fpgaconvnet.proto.fpgaconvnet_pb2
import fpgaconvnet.tools.onnx_helper as onnx_helper

from fpgaconvnet.hls.generate.partition import GeneratePartition
from fpgaconvnet.hls.tools.onnx_data import ONNXData

# load partition info
partitions = fpgaconvnet.proto.fpgaconvnet_pb2.partitions()
with open("single_layer.json", "r") as f:
   json_format.Parse(f.read(), partitions)
partition = partitions.partition[0]

# load onnx_data for this partition
onnx_data = ONNXData(partition, "single_layer.onnx")

# create partition generator
single_layer = GeneratePartition("single_layer", partition, onnx_data, "partition_0")

# generate each part of the partition
single_layer.generate_layers()
single_layer.generate_weights()
single_layer.generate_streams()
single_layer.generate_include()
single_layer.generate_source()
single_layer.generate_testbench()

# create HLS project
single_layer.create_vivado_hls_project()

# run c-synthesis
single_layer.run_csynth()

# export design
single_layer.export_design()

