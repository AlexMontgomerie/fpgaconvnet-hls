import os
import random
import yaml
import numpy as np
import json
import sys, getopt

class Data:
    # Initialise Parameters
    def __init__(self,name):
        # parameters
        self.param = {}
        # module name
        self.name = name

    # Fixed-point representation
    def fixed_point(data,INT_WIDTH=4,FRAC_WIDTH=12):
        return int( int(data*(2**FRAC_WIDTH))&0xFFFF )

    # Data Generation
    def gen_data(self,dim,data_range=[0,1],data_type=float):
        # Initialise random data array
        data = np.ndarray(dim,dtype=data_type)
        # assign values
        for index,_ in np.ndenumerate(data):
            data[index] = random.uniform(data_range[0],data_range[1])
        return data

    # Stimulus Generation
    # NOTE: need to create for each testbench
    def gen_stimulus(self):
        return {}

    # Header File Generation
    def gen_param_header(self,header_path,data_path):
        # header top
        header = """#ifndef {NAME}_PARAM_HPP_
#define {NAME}_PARAM_HPP_

#define DATA_DIR "{data_path}"
""".format(NAME=self.name.upper(),data_path=data_path)
        # variables
        for key,val in self.param.items():
            if isinstance(val,list):
                for i in range(len(val)):
                    header += "#define {NAME}_{PARAM}_{INDEX} \t {val}\n".format(
                        NAME =self.name.upper(),
                        PARAM=key.upper(),
                        INDEX=i,
                        val  =val[i]
                    )
                header += "#define {NAME}_{PARAM}_LEN \t {len}\n".format(
                    NAME =self.name.upper(),
                    PARAM=key.upper(),
                    len  =len(val)
                )
                header += "#define {NAME}_{PARAM}_TOTAL \t {total}\n".format(
                    NAME =self.name.upper(),
                    PARAM=key.upper(),
                    total=sum(val)
                )


            else:
                header += "#define {NAME}_{PARAM} \t {val}\n".format(
                    NAME =self.name.upper(),
                    PARAM=key.upper(),
                    val  =val
                )

        header += "#endif"
        # save to file
        with open(header_path+"/{name}_param.hpp".format(name=self.name),"w") as f:
            f.write(header)

    def stream2AxiStream(self,stream,coarse,data_type=fixed_point):
        stream_out = np.zeros([int(len(stream)/coarse)],dtype=np.uint64)
        for i in range(int(len(stream)/coarse)):
            for j in range(coarse):
                stream_out[i] |=  np.uint64( data_type(stream[i*coarse+j]) << j*16 )
        return stream_out.tolist()

    def main(self,argv):


        config_path = ''
        output_path = ''
        header_path = ''

        try:
            opts,args = getopt.getopt(argv,"h:c:o:")
        except getopt.GetoptError:
            sys.exit(2)
        for opt,arg in opts:
            if opt in ('-c'):
                config_path = arg
            elif opt in ('-o'):
                output_path = arg
            elif opt in ('-h'):
                header_path = arg

        # load config
        with open(config_path,'r') as f:
            config = json.load(f)

        # add parameters
        for key,val in config.items():
            self.param[key] = val

        # generate signals
        data, model = self.gen_stimulus()

        # save data as .dat files
        for filename in data:
            with open(os.path.join(output_path, filename+".dat"), 'w') as f:
                f.write("\n".join([str(i) for i in data[filename]]))

        # add parameters
        for key,val in self.param.items():
            model[key] = val

        # save data
        with open(output_path+'/model.json', 'w') as f:
            json.dump(model, f)

        # save header file
        self.gen_param_header(header_path,output_path)

