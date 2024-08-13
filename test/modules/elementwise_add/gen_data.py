import os
import sys
import numpy as np

sys.path.append('..')

from fpgaconvnet.models.modules.EltWise import EltWise
from Data import Data

class EltWiseADDTB(Data):
    def __init__(self):
        Data.__init__(self,'elementwise_add')

    # update stimulus generation
    def gen_stimulus(self):

        # Init Module
        elementwise_add = EltWise(
            self.param['rows'],
            self.param['cols'],
            self.param['channels'], 
            2, 
            'add'
        )
        
        elementwise_add.eltwise_type = 'add'

        # add parameters
        self.param['data_width'] = elementwise_add.data_width
        self.param['data_int_width'] = elementwise_add.data_width//2

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
        data_out = elementwise_add.functional_model(data_in)

        # return data
        data = {
            'input_1'     : data_in[0].reshape(-1).tolist(),
            'input_2'     : data_in[1].reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }

        # resource and latency model
        model = {
            'latency'   : elementwise_add.latency(),
            'resources' : elementwise_add.rsc()
        }

        return data, model

if __name__ == '__main__':
    elementwise_add_tb = EltWiseADDTB()
    elementwise_add_tb.main(sys.argv[1:])

