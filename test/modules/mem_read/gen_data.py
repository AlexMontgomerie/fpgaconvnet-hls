import sys
import numpy as np
import math

sys.path.append('..')

from Data import Data

class MemReadTB(Data):
    def __init__(self):
        Data.__init__(self,'mem_read')

    # update stimulus generation
    def gen_stimulus(self):
        # data in
        data_in = self.gen_data([
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in']
        ])
        # latency
        latency = self.param['rows_in']*self.param['cols_in']*int(self.param['channels_in']/self.param["streams_in"])
        # return data
        data = {
            'data'     : data_in.reshape(-1).tolist(),
        }
        # resource and latency model
        model = {
            'latency'   : latency,
            'resources' : {"BRAM" : 0, "DSP" : 0, "LUT" : 0, "FF" : 0}
        }
        return data, model

if __name__ == '__main__':
    mem_read_tb = MemReadTB()
    mem_read_tb.main(sys.argv[1:])

