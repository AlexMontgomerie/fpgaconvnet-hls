import sys
import numpy as np
import math

sys.path.append('..')

from Data import Data

class MemWriteTB(Data):
    def __init__(self):
        Data.__init__(self,'mem_write')

    # update stimulus generation
    def gen_stimulus(self):
        # data in
        data_out = self.gen_data([
            self.param['weights_reloading_factor'],
            self.param['rows_out'],
            self.param['cols_out'],
            self.param['channels_out']
        ])
        # latency
        latency = self.param['rows_out']*self.param['cols_out']*int(self.param['channels_out']/self.param["streams_out"])
        # return data
        data = {
            'data'     : data_out.reshape(-1).tolist(),
        }
        # resource and latency model
        model = {
            'latency'   : latency,
            'resources' : {"BRAM" : 0, "DSP" : 0, "LUT" : 0, "FF" : 0}
        }
        return data, model

if __name__ == '__main__':
    mem_write_tb = MemWriteTB()
    mem_write_tb.main(sys.argv[1:])

