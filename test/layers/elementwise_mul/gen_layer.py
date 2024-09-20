import sys 
import os 
import numpy as np

sys.path.append("..")
from Layer import Layer

from fpgaconvnet.models.layers.EltWiseLayer import EltWiseLayer
import fpgaconvnet.hls.generate.layers.elementwise_mul as elementwise_mul

class EltWiseMulLayerTB(Layer):
    def __init__(self):
        self.name = 'elementwise_mul_layer'
        Layer.__init__(self,self.name)
        
    # update stimulus generation
    def gen_stimulus(self): 
        # Init Module
        layer = EltWiseLayer(
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in'],
            ports_in=2, 
            coarse=self.param['coarse'],
            op_type='mul'
        )
        
        # data in
        data_in = np.zeros([
            2, 
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in']
        ])
        
        for ports_in in range (2): 
            data_in[ports_in] = self.gen_data([
                self.param['rows_in'],
                self.param['cols_in'],
                self.param['channels_in']
            ],data_range=[-1,1])
            
        # data out
        assert layer.op_type == 'mul', f"Operation type cannot be {layer.op_type}, must be mul"
        data_out = layer.functional_model(data_in)[0]
        data_out = np.moveaxis(data_out,0,-1)
        
        # add output dimensions
        self.param['rows_out']      = layer.rows_out()
        self.param['cols_out']      = layer.cols_out()
        self.param['channels_out']  = layer.channels_out()
        # return data
        data = {
            'input_1'  : data_in[0].reshape(-1).tolist(),
            'input_2'  : data_in[1].reshape(-1).tolist(),
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
        elementwise_mul.gen_elementwise_mul_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )
        
if __name__ == '__main__':
    elementwise_add_layer = EltWiseMulLayerTB()
    elementwise_add_layer.main(sys.argv[1:])