import os
import sys

sys.path.append('..')

from fpgaconvnet.models.modules.Fork import Fork
from Data import Data

class ForkTB(Data):
    def __init__(self):
        Data.__init__(self,'fork')

    # update stimulus generation
    def gen_stimulus(self):

        # Init Module
        fork = Fork(
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['kernel_size'],
            self.param['coarse']
        )

        # add parameters
        self.param['data_width'] = fork.data_width
        self.param['data_int_width'] = fork.data_width//2

        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['kernel_size'][0],
            self.param['kernel_size'][1]
        ])

        # data out
        data_out = fork.functional_model(data_in)

        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }

        # resource and latency model
        model = {
            'latency'   : fork.latency(),
            'resources' : fork.rsc()
        }

        return data, model

if __name__ == '__main__':
    fork_tb = ForkTB()
    fork_tb.main(sys.argv[1:])

