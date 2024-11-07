import onnx
import numpy as np
from PIL import Image

from fpgaconvnet.hls.generate.network import GenerateNetwork

from fpgaconvnet.parser import Parser
import fpgaconvnet.parser.onnx.helper as onnx_helper


# create instance of the network
net = GenerateNetwork("single_layer", "single_layer.json", "single_layer.onnx")

# load test data
input_image = Image.open("../mnist_example.png")

# transform image
# input_image = input_image.resize((224, 224), Image.ANTIALIAS)

# to a numpy array
input_image = np.array(input_image, dtype=np.float32)

# add channel dimension
input_image = np.expand_dims(input_image, axis=0)

# normalise
input_image = input_image/np.linalg.norm(input_image)

# duplicate across batch size
input_image = np.stack([input_image for _ in range(1)], axis=0 )

# create project for first partition
net.create_partition_project(0)

# # run the partition's testbench
# net.run_testbench(0, input_image)

# generate hardware
net.generate_partition_hardware(0)

# run co-simulation
net.run_cosimulation(0, input_image)
