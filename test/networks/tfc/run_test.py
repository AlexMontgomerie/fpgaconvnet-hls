import onnx
import numpy as np
from PIL import Image

from fpgaconvnet.hls.generate.network import GenerateNetwork

import fpgaconvnet.tools.onnx_helper as onnx_helper

# create instance of the network
net = GenerateNetwork("tfc", "tfc.json", "tfc.onnx")

# load test data
input_image = Image.open("../mnist_example.png")

# transform image
# input_image = input_image.resize((224, 224), Image.ANTIALIAS)

# to a numpy array
input_image = np.array(input_image, dtype=np.float32)

# flatten to 1 dimension
input_image = input_image.flatten()

# normalise
input_image = input_image/np.linalg.norm(input_image)

# duplicate across batch size
input_image = np.stack([input_image for _ in range(1)], axis=0 )

# create project for first partition
net.create_partition_project(0)

# run the partition's testbench
net.run_testbench(0, input_image)

# generate hardware
net.generate_partition_hardware(0)

# run co-simulation
net.run_cosimulation(0, input_image)
