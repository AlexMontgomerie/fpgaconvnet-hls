import os
import sys

sys.path.append('..')

from fpgaconvnet.models.modules.ReLU import ReLU
from Data import Data

class ReLUTB(Data):
    def __init__(self):
        Data.__init__(self,'relu')

    # update stimulus generation
    def gen_stimulus(self):

        # Init Module
        relu = ReLU(
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        )

        # add parameters
        self.param['data_width'] = relu.data_width
        self.param['data_int_width'] = relu.data_width//2

        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])

        # data out
        data_out = relu.functional_model(data_in)

        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }

        # resource and latency model
        model = {
            'latency'   : relu.latency(),
            'resources' : relu.rsc()
        }

        return data, model

if __name__ == '__main__':
    relu_tb = ReLUTB()
    relu_tb.main(sys.argv[1:])

