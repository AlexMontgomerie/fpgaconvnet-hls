import sys
import os
import numpy as np
import copy

sys.path.append('..')

from fpgaconvnet.models.layers.GlobalPoolingLayer import GlobalPoolingLayer
import fpgaconvnet.hls.generate.layers.global_pooling as global_pooling
from Layer import Layer

class GlobalPoolingLayerTB(Layer):
    def __init__(self):
        self.name = 'global_pooling_layer'
        Layer.__init__(self,self.name)
        
    # update stimulus generation
    def gen_stimulus(self): 
        
        # Init Layer
        layer = GlobalPoolingLayer(
            rows=self.param['rows_in'],
            cols=self.param['cols_in'],
            channels=self.param['channels_in'],
            coarse=self.param['coarse'],
        )
        
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
        global_pooling.gen_global_pooling_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )
        
if __name__ == "__main__":
    global_pooling_layer_tb = GlobalPoolingLayerTB()
    global_pooling_layer_tb.main(sys.argv[1:])
        