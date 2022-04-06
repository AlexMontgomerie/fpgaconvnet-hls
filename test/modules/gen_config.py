import os
import sys
import json
import random

sys.path.append('../..')

import transforms.coarse
import transforms.fine
import transforms.weights_reloading

import tools.parser as parser
from tools.layer_enum import LAYER_TYPE

NUM_CONFIG = 10

class GenConfigModule:

    def __init__(self,module_type,networks):

        self.module_type = module_type
        self.networks    = networks
        self.modules     = []

    def get_all_modules(self):
        # check over all networks
        for net in self.networks:
            # parse network
            _,node_info = parser.parse_net(net,view=False)
            # get all layers of the same type
            for layer in node_info:
                for module in node_info[layer]['hw'].modules:
                    if module == self.module_type:
                        # randomise layer
                        transforms.coarse.apply_random_coarse_layer(node_info[layer])
                        transforms.fine.apply_random_fine_layer(node_info[layer])
                        transforms.weights_reloading.apply_random_weights_reloading_layer(node_info[layer])
                        node_info[layer]['hw'].update()
                        # get a description
                        net_name = os.path.basename(net)
                        desc = "module name: {}, layer name: {}, net name: {}".format(module,layer,net_name)
                        self.modules.append([node_info[layer]['hw'].modules[module],desc])

    def save_config(self,output_path):
        for module in self.modules:
            # get all fields
            module_info = module[0].module_info()
            module_info['batch_size']  = 1
            module_info['description'] = module[1]
            with open(os.path.join(output_path,'config/config_{}.json'.format(self.modules.index(module))),'w') as f:
                json.dump(module_info,f,indent=4)

if __name__ == "__main__":
    # 
    networks = [
        "../../data/models/lenet.prototxt",
        "../../data/models/alexnet.prototxt",
        "../../data/models/googlenet.prototxt",
        "../../data/models/resnet.prototxt",
        "../../data/models/vgg16.prototxt",
    ]
   
    # Accum 
    accum_config = GenConfigModule('accum',networks)
    accum_config.get_all_modules()
    accum_config.modules = random.sample(accum_config.modules, NUM_CONFIG)
    accum_config.save_config("accum")

    # Batch Norm
    bn_config = GenConfigModule('batch_norm',networks)
    bn_config.get_all_modules()
    bn_config.modules = random.sample(bn_config.modules, NUM_CONFIG)
    bn_config.save_config("batch_norm")

    # Conv
    conv_config = GenConfigModule('conv',networks)
    conv_config.get_all_modules()
    conv_config.modules = random.sample(conv_config.modules, NUM_CONFIG)
    conv_config.save_config("conv")

    # Fork
    fork_config = GenConfigModule('fork',networks)
    fork_config.get_all_modules()
    fork_config.modules = random.sample(fork_config.modules, NUM_CONFIG)
    fork_config.save_config("fork")

    # Glue
    glue_config = GenConfigModule('glue',networks)
    glue_config.get_all_modules()
    glue_config.modules = random.sample(glue_config.modules, NUM_CONFIG)
    glue_config.save_config("glue")

    # Pool
    pool_config = GenConfigModule('pool',networks)
    pool_config.get_all_modules()
    pool_config.modules = random.sample(pool_config.modules, NUM_CONFIG)
    pool_config.save_config("pool")

    # Relu
    relu_config = GenConfigModule('relu',networks)
    relu_config.get_all_modules()
    relu_config.modules = random.sample(relu_config.modules, NUM_CONFIG)
    relu_config.save_config("relu")

    # Sliding Window
    sw_config = GenConfigModule('sliding_window',networks)
    sw_config.get_all_modules()
    sw_config.modules = random.sample(sw_config.modules, NUM_CONFIG)
    sw_config.save_config("sliding_window")


