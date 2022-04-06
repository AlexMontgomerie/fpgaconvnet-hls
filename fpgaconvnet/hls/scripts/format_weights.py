import sys
import tempfile
import parser
import argparse
import json
import numpy as np
import random
import os
import onnx
from PIL import Image
from google.protobuf import json_format

sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

import fpgaconvnet_optimiser.proto.fpgaconvnet_pb2
import fpgaconvnet_optimiser.tools.onnx_helper as onnx_helper

from tools.onnx_data import ONNXData

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Weights Formatting Script")
    parser.add_argument('-p','--partition_path',metavar='PATH',required=True,
        help='Path to partition info (.pb.bin)')
    parser.add_argument('-m','--onnx_path',metavar='PATH',required=False,
        help='Path to onnx model (.onnx)')
    parser.add_argument('-i','--partition_index',metavar='N',required=True, type=int,
        help='Partition index')

    # parse arguments
    args = parser.parse_args()

    # load partition information
    partitions = fpgaconvnet_optimiser.proto.fpgaconvnet_pb2.partitions()
    with open(args.partition_path,'r') as f:
        json_format.Parse(f.read(), partitions)

    # onnx data manipulation
    onnx_data = ONNXData(partitions.partition[args.partition_index], args.onnx_path)

    ## save weight coefficients ##
    onnx_data.save_weights_partition(
        f'partition_{args.partition_index}/data',
        to_yaml=True,
        to_csv=True,
        to_bin=True,
        to_dat=True)

