import sys
import os
import numpy as np
import csv
import copy

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

from fpgaconvnet_optimiser.models.layers.InnerProductLayer import InnerProductLayer
import generate.layers.inner_product
from Layer import Layer
from tools.onnx_data import ONNXData
from tools.array_init import array_init

class InnerProductLayerTB(Layer):
    def __init__(self):
        self.name = 'inner_product_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        layer = InnerProductLayer(
            self.param['filters'],
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in'],
            coarse_in=self.param['coarse_in'],
            coarse_out=self.param['coarse_out'],
            input_width=self.param["input_width"],
            output_width=self.param["output_width"],
            acc_width=self.param["acc_width"],
            weight_width=self.param["weight_width"]
        )

        # data in
        data_in = self.gen_data([
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in']
        ])

        # weights
        weights = self.gen_data([
            self.param['filters'],
            self.param['cols_in']*self.param['rows_in']*self.param['channels_in']
        ])
        bias     = np.zeros(self.param['filters'])

        # data out
        data_out = layer.functional_model(copy.copy(data_in),weights,bias)[0]
        data_out = np.moveaxis(data_out,0,-1)

        # reshape weights
        weights = np.reshape(weights,(self.param['filters'],self.param['cols_in'],self.param['rows_in'],self.param['channels_in'],1,1))
        weights = np.rollaxis(weights,1,3)
        weights = np.reshape(weights,(self.param['filters'],self.param['cols_in']*self.param['rows_in']*self.param['channels_in'],1,1))

        # save weights
        weights = ONNXData._transform_weights(
            weights,
            1,
            self.param['coarse_in'],
            self.param['coarse_out'],
            1,
            1
        )
        with open('data/weights.csv', 'w') as f:
            f.write(array_init(weights[0]))

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
        generate.layers.inner_product.gen_inner_product_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    inner_product_layer_tb = InnerProductLayerTB()
    inner_product_layer_tb.main(sys.argv[1:])
