import onnx
import numpy as np
from PIL import Image

from fpgaconvnet.hls.generate.network import GenerateNetwork

import fpgaconvnet.tools.onnx_helper as onnx_helper

# create instance of the network
net = GenerateNetwork("single_layer", "single_layer.json", "single_layer.onnx")

# load test data
input_image = Image.open("../mnist_example.png")

# remove biases
for layer in net.partitions.partition[0].layers:
    if layer.bias_path:
        initializer = onnx_helper.get_model_initializer(net.model, layer.bias_path, to_tensor=False)
        # TODO: seems like theres no bias initializer for inner product layer
        print(initializer)
        if not initializer:
            continue
        zeroes = np.zeros(onnx.numpy_helper.to_array(initializer).shape).astype(np.float32)
        initializer_new = onnx.numpy_helper.from_array(zeroes,name=initializer.name)
        net.model.graph.initializer.remove(initializer)
        net.model.graph.initializer.extend([initializer_new])

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

print(input_image.shape)
# create project for first partition
net.create_partition_project(0)

# run the partition's testbench
net.run_testbench(0, input_image)

# net.generate_partition_hardware(0)
