import os
import sys

sys.path.append('..')

from fpgaconvnet.models.modules.SlidingWindow import SlidingWindow
from Data import Data

class SlidingWindowTB(Data):
    def __init__(self):
        Data.__init__(self,'sliding_window')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        sliding_window = SlidingWindow(
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['kernel_size'],
            self.param['stride'],
            self.param['pad_top'],
            self.param['pad_right'],
            self.param['pad_bottom'],
            self.param['pad_left']
        )

        # add parameters
        self.param['data_width'] = sliding_window.data_width
        self.param['data_int_width'] = sliding_window.data_width//2

        # output dimensions
        self.param['rows_out'] = sliding_window.rows_out()
        self.param['cols_out'] = sliding_window.cols_out()

        # data in
        data_in = self.gen_data([
            self.param['batch_size'],
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])
        # data out
        data_out = sliding_window.functional_model(data_in)
        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : sliding_window.latency(),
            'resources' : sliding_window.rsc()
        }
        return data, model

if __name__ == '__main__':
    sliding_window_tb = SlidingWindowTB()
    sliding_window_tb.main(sys.argv[1:])

