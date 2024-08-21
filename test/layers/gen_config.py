# This script is out of date and yet to be updated
import os
import sys
import json
import random

sys.path.append('../..')

import tools.parser as parser
import transforms.coarse
import transforms.fine
import transforms.weights_reloading
from tools.layer_enum import LAYER_TYPE

NUM_CONFIG = 10

class GenConfigLayer:

    def __init__(self,layer_type,networks):

        self.layer_type = layer_type
        self.networks   = networks
        self.layers     = []

    def get_all_layers(self):
        # check over all networks
        for net in self.networks:
            # parse network
            _,node_info = parser.parse_net(net,view=False)
            # get all layers of the same type
            for layer in node_info:
                if node_info[layer]['type'] == self.layer_type:
                    # randomise layer
                    transforms.coarse.apply_random_coarse_layer(node_info[layer])
                    transforms.fine.apply_random_fine_layer(node_info[layer])
                    transforms.weights_reloading.apply_random_weights_reloading_layer(node_info[layer])
                    node_info[layer]['hw'].update()
                    # get a description
                    net_name = os.path.basename(net)
                    desc = "layer name: {}, net name: {}".format(layer,net_name)
                    self.layers.append([node_info[layer]['hw'],desc])

    def save_config(self,output_path):
        for layer in self.layers:
            # get all fields
            layer_info = layer[0].layer_info()
            layer_info['batch_size']  = 1
            layer_info['description'] = layer[1]
            with open(os.path.join(output_path,'config/config_{}.json'.format(self.layers.index(layer))),'w') as f:
                json.dump(layer_info,f,indent=4)

if __name__ == "__main__":
    # 
    networks = [
        "../../data/models/lenet.prototxt",
        "../../data/models/alexnet.prototxt",
        "../../data/models/googlenet.prototxt",
        "../../data/models/resnet.prototxt",
        "../../data/models/vgg16.prototxt",
    ]
   
    # Batch Norm
    bn_config = GenConfigLayer(LAYER_TYPE.BatchNorm,networks)
    bn_config.get_all_layers()
    bn_config.layers = random.sample(bn_config.layers, NUM_CONFIG)
    bn_config.save_config("batch_norm")

    # Concat
    concat_config = GenConfigLayer(LAYER_TYPE.Concat,networks)
    concat_config.get_all_layers()
    #concat_config.layers = random.sample(concat_config.layers, NUM_CONFIG)
    concat_config.save_config("concat")

    # Convolution
    conv_config = GenConfigLayer(LAYER_TYPE.Convolution,networks)
    conv_config.get_all_layers()
    conv_config.layers = random.sample(conv_config.layers, NUM_CONFIG)
    conv_config.save_config("convolution")

    # Eltwise
    eltwise_config = GenConfigLayer(LAYER_TYPE.Eltwise,networks)
    eltwise_config.get_all_layers()
    #eltwise_config.layers = random.sample(eltwise_config.layers, NUM_CONFIG)
    eltwise_config.save_config("eltwise")

    # Inner Product
    ip_config = GenConfigLayer(LAYER_TYPE.InnerProduct,networks)
    ip_config.get_all_layers()
    #ip_config.layers = random.sample(ip_config.layers, NUM_CONFIG)
    ip_config.save_config("inner_product")

    # Pooling
    pool_config = GenConfigLayer(LAYER_TYPE.Pooling,networks)
    pool_config.get_all_layers()
    pool_config.layers = random.sample(pool_config.layers, NUM_CONFIG)
    pool_config.save_config("pooling")

    # ReLU
    relu_config = GenConfigLayer(LAYER_TYPE.ReLU,networks)
    relu_config.get_all_layers()
    relu_config.layers = random.sample(relu_config.layers, NUM_CONFIG)
    relu_config.save_config("relu")
 
