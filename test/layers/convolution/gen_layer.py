import sys
import os
import numpy as np
import csv
import copy

sys.path.append("..")

from fpgaconvnet.models.layers.ConvolutionLayer import ConvolutionLayer
import fpgaconvnet.hls.generate.layers.convolution as generate_convolution
from Layer import Layer
from fpgaconvnet.hls.tools.onnx_data import _transform_weights
from fpgaconvnet.hls.tools.onnx_data import _transform_biases
from fpgaconvnet.hls.tools.array_init import array_init

class ConvolutionLayerTB(Layer):
    def __init__(self):
        self.name = 'convolution_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):

        # Init Module
        layer = ConvolutionLayer(
            self.param['filters'],
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in'],
            kernel_size=self.param['kernel_size'],
            stride=self.param['stride'],
            groups=self.param['groups'],
            pad=self.param['pad'],
            coarse_in=self.param['coarse_in'],
            coarse_out=self.param['coarse_out'],
            coarse_group=self.param['coarse_group'],
            fine=self.param['fine'],
            input_width=self.param["input_width"],
            output_width=self.param["output_width"],
            acc_width=self.param["acc_width"],
            weight_width=self.param["weight_width"]
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
        # weights
        weights = self.gen_data([
            self.param['filters'],
            self.param['channels_in']//self.param['groups'],
            self.param['kernel_size'][0],
            self.param['kernel_size'][1]
        ],[-8,8]) #todo: consistent with weight_t

        # create bias data
        bias = self.gen_data([
            self.param['filters']
        ],[-8,8]) #todo: consistent with biases_t

        # data out
        data_out = layer.functional_model(copy.copy(data_in),weights,bias)[0]
        data_out = np.moveaxis(data_out,0,-1)

        # save weights
        weights = _transform_weights(
            weights,
            1,
            self.param['coarse_in'],
            self.param['coarse_out'],
            self.param['coarse_group'],
            self.param['groups']
        )
        with open('data/weights.csv', 'w') as f:
            f.write(array_init(weights[0]))

        # save biases
        bias = _transform_biases(
            bias,
            self.param['filters'],
            self.param['coarse_out']
        )
        with open('data/biases.csv', 'w') as f:
            f.write(array_init(bias[0]))

        # add output dimensions
        self.param['rows_out']      = layer.rows_out()
        self.param['cols_out']      = layer.cols_out()
        self.param['channels_out']  = layer.channels_out()

        # data_in = data_in.reshape(
        #     self.param['rows_in'],
        #     self.param['cols_in'],
        #     int(self.param['groups']/self.param['coarse_group']),
        #     self.param['coarse_group'],
        #     int(self.param['channels_in']/(self.param['groups']*self.param['coarse_in'])),
        #     self.param['coarse_in']
        # )
        # data_in = data_in.transpose((0,1,2,4,3,5))

        # data_out = data_out.reshape(
        #     self.param['rows_out'],
        #     self.param['cols_out'],
        #     int(self.param['groups']/self.param['coarse_group']),
        #     self.param['coarse_group'],
        #     int(self.param['channels_out']/(self.param['groups']*self.param['coarse_out'])),
        #     self.param['coarse_out']
        # )
        # data_out = data_out.transpose(0,1,2,4,3,5)

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
        generate_convolution.gen_convolution_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    convolution_layer_tb = ConvolutionLayerTB()
    convolution_layer_tb.main(sys.argv[1:])
