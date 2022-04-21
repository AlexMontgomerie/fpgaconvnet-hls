from google.protobuf import json_format

import fpgaconvnet.proto.fpgaconvnet_pb2
import fpgaconvnet.tools.onnx_helper as onnx_helper

from fpgaconvnet.hls.generate.network import GenerateNetwork

net = GenerateNetwork("single_layer", "single_layer.json", "single_layer.onnx")

net.generate_partition(0)
