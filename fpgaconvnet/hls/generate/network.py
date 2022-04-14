from google.protobuf import json_format

import fpgaconvnet.proto.fpgaconvnet_pb2
import fpgaconvnet.tools.onnx_helper as onnx_helper

from fpgaconvnet.hls.tools.onnx_data import ONNXData
from fpgaconvnet.hls.generate.partition import GeneratePartition

class GenerateNetwork:

    def __init__(self, partition_path, model_path):

        # load partition information
        self.partitions = fpgaconvnet.proto.fpgaconvnet_pb2.partitions()
        with open(args.partition_path,'r') as f:
           json_format.Parse(f.read(), partitions)

        # create generator for each partition
        # TODO
