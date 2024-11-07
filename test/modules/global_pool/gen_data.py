import sys

sys.path.append('..')

from fpgaconvnet.models.modules.GlobalPool import GlobalPool
from Data import Data

class GlobalPoolTB(Data):
    def __init__(self): 
        Data.__init__(self,'global_pool')
        
    # update stimulus generation
    def gen_stimulus(self): 
        
        # Init Module
        global_pool = GlobalPool(
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        )
        
        # add parameters
        self.param['data_width'] = global_pool.data_width
        self.param['data_int_width'] = global_pool.data_width//2
        
        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])
        
        data_out = global_pool.functional_model(data_in)
        
        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }
        
        # resource and latency model
        model = {
            'latency'   : global_pool.latency(),
            'resources' : global_pool.rsc()
        }
        
        return data, model
    
if __name__ == '__main__':
    global_pool_tb = GlobalPoolTB()
    global_pool_tb.main(sys.argv[1:])