import os
import sys
import numpy as np

sys.path.append('..')

from fpgaconvnet.models.modules.EltWise import EltWise
from Data import Data

class EltWiseMULTB(Data):
    def __init__(self):
        Data.__init__(self,'elementwise_mul')

    # update stimulus generation
    def gen_stimulus(self):

        # Init Module
        elementwise_mul = EltWise(
            self.param['rows'],
            self.param['cols'],
            self.param['channels'], 
            2, 
            'mul'
        )
        
        elementwise_mul.eltwise_type = 'mul'

        # add parameters
        self.param['data_width'] = elementwise_mul.data_width
        self.param['data_int_width'] = elementwise_mul.data_width//2

        # data in
        data_in = np.ndarray(shape=(
            2, 
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ), dtype=float)
        
        data_in[0] = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])
        
        data_in[1] = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])

        # data out
        data_out = elementwise_mul.functional_model(data_in)

        # return data
        data = {
            'input_1'     : data_in[0].reshape(-1).tolist(),
            'input_2'     : data_in[1].reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }

        # resource and latency model
        model = {
            'latency'   : elementwise_mul.latency(),
            'resources' : elementwise_mul.rsc()
        }

        return data, model

if __name__ == '__main__':
    elementwise_mul_tb = EltWiseMULTB()
    elementwise_mul_tb.main(sys.argv[1:])

