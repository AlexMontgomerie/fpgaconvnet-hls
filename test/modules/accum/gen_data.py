import os
import sys

sys.path.append('..')

from fpgaconvnet.models.modules.Accum import Accum
from Data import Data

class AccumTB(Data):
    def __init__(self):
        Data.__init__(self,'accum')

    # update stimulus generation
    def gen_stimulus(self):

        self.param['channels_per_group'] = int(self.param['channels']/self.param['groups'])
        self.param['filters_per_group'] = int(self.param['filters'] /self.param['groups'])

        # Init Module
        accum = Accum(
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['filters'],
            self.param['groups']
        )

        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['filters_per_group']
        ])

        # add parameters
        self.param['data_width'] = accum.data_width
        self.param['data_int_width'] = accum.data_width//2

        # data out
        data_out = accum.functional_model(data_in)

        # return data
        data = {
            'input'  : data_in.reshape(-1).tolist(),
            'output' : data_out.reshape(-1).tolist()
        }

        # resource and latency model
        model = {
            'latency'   : accum.latency(),
            'resources' : accum.rsc()
        }

        return data, model

if __name__ == '__main__':
    accum_tb = AccumTB()
    accum_tb.main(sys.argv[1:])

