import sys
import os
import numpy as np
import csv
import copy

sys.path.append('..')

from fpgaconvnet.models.layers.PoolingLayer import PoolingLayer
import fpgaconvnet.hls.generate.layers.pooling as pooling
from Layer import Layer

class PoolingLayerTB(Layer):
    def __init__(self):
        self.name = 'pooling_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):

        # Init Layer
        layer = PoolingLayer(
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in'],
            coarse=self.param['coarse'],
            pool_type=self.param['pool_type'],
            kernel_rows=self.param['kernel_size'],
            kernel_cols=self.param['kernel_size'],
            stride_rows=self.param['stride'],
            stride_cols=self.param['stride']
        )

        # update parameters
        self.param["kernel_size"] = layer.kernel_size
        self.param["stride"] = layer.stride
        self.param["pad_top"] = layer.pad_top
        self.param["pad_right"] = layer.pad_right
        self.param["pad_bottom"] = layer.pad_bottom
        self.param["pad_left"] = layer.pad_left

        # data in
        data_in = self.gen_data([
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in']
        ])

        # data out
        data_out = layer.functional_model(copy.copy(data_in))[0]
        data_out = np.moveaxis(data_out,0,-1)

        # add output dimensions
        self.param['rows_out']      = layer.rows_out()
        self.param['cols_out']      = layer.cols_out()
        self.param['channels_out']  = layer.channels_out()

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
        pooling.gen_pooling_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    pooling_layer_tb = PoolingLayerTB()
    pooling_layer_tb.main(sys.argv[1:])
