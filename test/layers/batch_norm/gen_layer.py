import sys
import os
import numpy as np
import csv

sys.path.append('..')

from fpgaconvnet_optimiser.models.layers.BatchNormLayer import BatchNormLayer

import generate.layers.batch_norm
from Layer import Layer
from tools.onnx_data import ONNXData

class BatchNormLayerTB(Layer):
    def __init__(self):
        self.name = 'batch_norm_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        layer = BatchNormLayer(
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['coarse'],
            self.param['coarse']
        )

        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])

        # batch norm coefficients
        gamma = self.gen_data([self.param['channels']])
        beta  = self.gen_data([self.param['channels']])
        eps   = np.float_power(10,-5)

        # data out
        data_out, mean, var = layer.functional_model(data_in,gamma,beta)
        data_out = np.moveaxis(data_out,0,-1)

        # get hardware coefficients
        scale = gamma / np.sqrt( var + eps )
        shift = np.divide(beta,scale) - mean

        # save scale
        scale = CaffeData().transform_batch_norm_coef(
            scale,
            self.param['coarse']
        )
        with open('data/scale.csv', 'w') as f:
            writer = csv.writer(f)
            writer.writerows([scale.reshape(-1).tolist()])

        # save shift
        shift = CaffeData().transform_batch_norm_coef(
            shift,
            self.param['coarse']
        )
        with open('data/shift.csv', 'w') as f:
            writer = csv.writer(f)
            writer.writerows([shift.reshape(-1).tolist()])

        # add output dimensions
        self.param['rows_out']      = layer.rows_out()
        self.param['cols_out']      = layer.cols_out()
        self.param['channels_out']  = layer.channels_out()

        # return data
        # return data
        data = {
            'input'  : data_in.reshape(-1).tolist(),
            'output' : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : layer.latency(),
            'resources' : layer.resource()
        }
        return data, model

    # update layer generation
    def gen_layer(self,src_path,header_path):
        generate.layers.batch_norm.gen_batch_norm_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    batch_norm_layer_tb = BatchNormLayerTB()
    batch_norm_layer_tb.main(sys.argv[1:])
