import os
import sys

sys.path.append('..')

from fpgaconvnet.models.modules.Pool import Pool
from Data import Data

class AvgPoolTB(Data):
    def __init__(self):
        Data.__init__(self,'avg_pool')

    # update stimulus generation
    def gen_stimulus(self):

        # Init Module
        if self.param['pool_type'] == 0:
            pool_type = 'max'
        if self.param['pool_type'] == 1:
            pool_type = 'avg'
        avg_pool = Pool(
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['kernel_size'],
            pool_type
        )

        # add parameters
        self.param['data_width'] = avg_pool.data_width
        self.param['data_int_width'] = avg_pool.data_width//2

        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['kernel_size'][0],
            self.param['kernel_size'][1]
        ])

        # data out
        data_out = avg_pool.functional_model(data_in)

        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }

        # resource and latency model
        model = {
            'latency'   : avg_pool.latency(),
            'resources' : avg_pool.rsc()
        }

        return data, model

if __name__ == '__main__':
    pool_tb = AvgPoolTB()
    pool_tb.main(sys.argv[1:])

