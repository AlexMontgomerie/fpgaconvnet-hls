import os
import sys

sys.path.append('..')

from fpgaconvnet.models.modules.Bias import Bias
from Data import Data

class BiasTB(Data):
    def __init__(self):
        Data.__init__(self,'bias')

    # update stimulus generation
    def gen_stimulus(self):

        # Init Module
        bias = Bias(
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['filters']
        )

        # add parameters
        self.param['data_width'] = bias.data_width
        self.param['data_int_width'] = bias.data_width//2
        self.param['biases_width'] = bias.biases_width
        self.param['biases_int_width'] = bias.biases_width//2

        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['filters']
        ])
        # biases
        biases = self.gen_data([
            self.param['filters']
        ])

        # data out
        data_out = bias.functional_model(data_in, biases)

        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),
            'biases'    : biases.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }

        # resource and latency model
        model = {
            'latency'   : bias.latency(),
            'resources' : bias.rsc()
        }
        return data, model

if __name__ == '__main__':
    bias_tb = BiasTB()
    bias_tb.main(sys.argv[1:])
