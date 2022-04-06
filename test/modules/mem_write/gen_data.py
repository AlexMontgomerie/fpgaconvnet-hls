import sys
import numpy as np
import math

sys.path.append('..')

DMA_WIDTH=64
DATA_WIDTH=16
STREAMS_PER_PORT=int(DMA_WIDTH/DATA_WIDTH)

from Data import Data

class MemWriteTB(Data):
    def __init__(self):     
        Data.__init__(self,'mem_write')

    # update stimulus generation
    def gen_stimulus(self):
        # data in & out 
        data = self.gen_data([
            self.param['rows_out']*self.param['cols_out']*self.param['channels_out']*self.param['weights_reloading_factor'],
            self.param['streams_out']
        ])
        # latency 
        latency = self.param['rows_out']*self.param['cols_out']*self.param['channels_out']
        # return data
        return {
            'data_in'  : data.reshape(-1).tolist(),
            'data_out' : { 'data' : data.reshape(-1).tolist() },
            'latency'  : latency
        }

if __name__ == '__main__':
    mem_write_tb = MemWriteTB()
    mem_write_tb.main(sys.argv[1:])    
 
