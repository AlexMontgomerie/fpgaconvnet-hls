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

#import optimiser.graphs
import generate.partition



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Hardware Generation Script")
    parser.add_argument('-n','--name',required=True,
        help='Name of network')
    parser.add_argument('-p','--partition_path',metavar='PATH',required=True,
        help='Path to partition info (.json)')
    #parser.add_argument('-m','--onnx_path',metavar='PATH',required=True,
    #    help='Path to onnx model (.onnx)')
    parser.add_argument('-i','--partition_index',metavar='N',required=True, type=int,
        help='Partition index')

    # parse arguments
    args = parser.parse_args()

    # load partition information
    partitions = fpgaconvnet_optimiser.proto.fpgaconvnet_pb2.partitions()
    with open(args.partition_path,'r') as f:
        json_format.Parse(f.read(), partitions)

    # generate network
    generate.partition.gen_network(args.name, partitions.partition[args.partition_index], f"partition_{args.partition_index}")

    # # iterate over partitions
    # if args.partition_index:
    #     # generate network
    #     generate.partition.gen_network(args.name, partitions.partition[args.partition_index], f"partition_{args.partition_index}")
    # else:
    #     for i, partition in enumerate(partitions.partition):
    #         # generate network
    #         generate.partition.gen_network(args.name, partition, f"partition_{i}")

