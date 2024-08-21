import sys
import os
import numpy as np

sys.path.append("..")
from Layer import Layer

from fpgaconvnet.models.layers.SplitLayer import SplitLayer
import fpgaconvnet.hls.generate.layers.split as split

class SplitLayerTB(Layer):
    def __init__(self):
        self.name = 'split_layer'
        Layer.__init__(self,self.name)
        
    # update stimulus generation
    def gen_stimulus(self): 
        # Init Module 
        layer = SplitLayer(
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in'],
            coarse=self.param['coarse']
        )
        
        # data in
        data_in = self.gen_data([
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in']
        ],data_range=[-1,1])
        # data out
        data_out = layer.functional_model(data_in)[0]
        
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
        split.gen_split_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )
        
if __name__ == '__main__':
    split_layer_tb = SplitLayerTB()
    split_layer_tb.main(sys.argv[1:])