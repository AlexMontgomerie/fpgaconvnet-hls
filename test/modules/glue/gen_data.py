import os
import sys

sys.path.append('..')

from fpgaconvnet.models.modules.Glue import Glue
from Data import Data

class GlueTB(Data):
    def __init__(self):
        Data.__init__(self,'glue')

    # update stimulus generation
    def gen_stimulus(self):

        # Init Module
        glue = Glue(
            self.param['rows'],
            self.param['cols'],
            1,
            self.param['filters'],
            self.param['coarse_in'],
            self.param['coarse_out']
        )

        # add parameters
        self.param['data_width'] = glue.data_width
        self.param['data_int_width'] = glue.data_width//2
        self.param['acc_width'] = glue.acc_width
        self.param['acc_int_width'] = glue.acc_width//2

        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            int(self.param['filters']/self.param['coarse_out']),
            self.param['coarse_in'],
            self.param['coarse_out']
        ])

        # data out
        data_out = glue.functional_model(data_in)

        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }

        # resource and latency model
        model = {
            'latency'   : glue.latency(),
            'resources' : glue.rsc()
        }

        return data, model

if __name__ == '__main__':
    glue_tb = GlueTB()
    glue_tb.main(sys.argv[1:])

