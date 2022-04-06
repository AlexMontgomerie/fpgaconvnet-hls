import os
import sys

sys.path.append('..')

from fpgaconvnet.models.modules.Conv import Conv
from Data import Data

class ConvTB(Data):
    def __init__(self):
        Data.__init__(self,'conv')

    # update stimulus generation
    def gen_stimulus(self):

        # add kernel size x and y
        self.param["kernel_size_x"] = self.param['kernel_size'][0]
        self.param["kernel_size_y"] = self.param['kernel_size'][1]

        # add channels and filters per group
        self.param["channels_per_group"] = self.param["channels"]//self.param["groups"]
        self.param["filters_per_group"]  = self.param["filters"]//self.param["groups"]

        # Init Module
        conv = Conv(
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['filters'],
            self.param['fine'],
            self.param['kernel_size'],
            self.param['groups']
        )

        # add parameters
        self.param['data_width'] = conv.data_width
        self.param['data_int_width'] = conv.data_width//2
        self.param['weight_width'] = conv.weight_width
        self.param['weight_int_width'] = conv.weight_width//2
        self.param['acc_width'] = conv.acc_width
        self.param['acc_int_width'] = conv.acc_width//2

        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['kernel_size'][0],
            self.param['kernel_size'][1]
        ])

        # weights
        weights = self.gen_data([
            self.param['channels'],
            int(self.param['filters']/self.param['groups']),
            self.param['kernel_size'][0],
            self.param['kernel_size'][1]
        ])

        # data out
        data_out = conv.functional_model(data_in, weights)

        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),
            'weights'   : weights.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }

        # resource and latency model
        model = {
            'latency'   : conv.latency(),
            'resources' : conv.rsc()
        }
        return data, model

if __name__ == '__main__':
    conv_tb = ConvTB()
    conv_tb.main(sys.argv[1:])

