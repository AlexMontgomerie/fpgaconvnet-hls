import yaml
import sys, os, getopt
import numpy as np
from PIL import Image
import copy
from types import FunctionType
import csv
import re
import random
from fpbinary import FpBinary
import math

import onnx
import onnxruntime
import onnx.numpy_helper

import fpgaconvnet.tools.graphs as graphs
import fpgaconvnet.tools.layer_enum as layer_enum
from fpgaconvnet.parser import Parser
import fpgaconvnet.parser.onnx.helper as onnx_helper
import fpgaconvnet.proto.fpgaconvnet_pb2 as fpgaconvnet_pb2

from fpgaconvnet.hls.tools.array_init import array_init

def fixed_point(val,total_width=16,int_width=8):
    val = min(val,  2**(int_width-1))
    val = max(val, -2**(int_width-1))
    return FpBinary(int_bits=int_width,frac_bits=(total_width-int_width),signed=True,value=val)

def get_layer_from_partition(partition, layer_name): # Non ONNXData class version
    for layer in partition.layers:
        if layer.name == layer_name:
            return layer

    #def load_input(self,filepath):
    #    self.data = np.array(Image.open(filepath),dtype=np.float32)
    #    #"normalising", more like scaling, input to prevent saturation of quant data types
    #    data_max = np.amax(self.data)
    #    self.data = self.data / data_max
    #    if len(self.data.shape) == 2:
    #       self.data = np.expand_dims(self.data,axis=0)
    #    self.data = np.stack([self.data for _ in range(self.partition.batch_size)], axis=0 )

    #def get_layer(self,layer_name):
    #    for layer in self.partition.layers:
    #        if layer.name == layer_name:
    #            return layer

    #def get_type(self,layer):
    #    return self.net.layers[list(self.net._layer_names).index(layer)].type

    #def gen_data(self,dim,data_range=[0,1],data_type=float):
    #    # Initialise random data array
    #    data = np.ndarray(dim,dtype=data_type)
    #    # assign values
    #    for index,_ in np.ndenumerate(data):
    #        data[index] = data_type(random.uniform(data_range[0],data_range[1]))
    #    return data

    #def normalise(self,data,scale=SCALE):
    #    return np.true_divide(data,scale)
    #    #return np.multiply(np.subtract(np.true_divide(data,scale),0.5),2)

    #def remove_initializer_from_input(self):
    #    inputs = self.model.graph.input
    #    name_to_input = {}
    #    for input in inputs:
    #        name_to_input[input.name] = input
    #    for initializer in self.model.graph.initializer:
    #        if initializer.name in name_to_input:
    #            inputs.remove(name_to_input[initializer.name])

    #def remove_bias(self):
    #    for layer in self.partition.layers:
    #        if layer.bias_path:
    #            initializer = onnx_helper.get_model_initializer(self.model, layer.bias_path, to_tensor=False)
    #            # TODO: seems like theres no bias initializer for inner product layer
    #            if not initializer:
    #                continue
    #            zeroes = np.zeros(onnx.numpy_helper.to_array(initializer).shape).astype(np.float32)
    #            initializer_new = onnx.numpy_helper.from_array(zeroes,name=initializer.name)
    #            self.model.graph.initializer.remove(initializer)
    #            self.model.graph.initializer.extend([initializer_new])

    """
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    DATA
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    """

def _convert_fixed_port_stream(stream_in, total_width=16, int_width=8):
    """
    converts the array in to fixed point
    """
    stream_out = np.ndarray(shape=stream_in.shape, dtype=FpBinary)
    for index,val in np.ndenumerate(stream_in):
        stream_out[index] = fixed_point(val,total_width=total_width,int_width=int_width)
    return stream_out

def _fixed_point_stream_format(stream, streams=1, port_width=64, ports=1):
    # check it's only a 1D array
    assert len(stream.shape) == 1
    # check the stream is fixed-point
    # TODO
    # get width of fixed point data
    data_width = sum(stream[0].format)
    # check theres enough ports for the streams
    print(f"Streams:{streams}, Ports:{ports}, PortWidth:{port_width}, DataWidth:{data_width}, Maximum stream number: {ports*(port_width/data_width)}, Number of elements = {stream.shape[0]}")
    if streams > ports*(port_width/data_width):
        raise ValueError(f"Streams:{streams}, Ports:{ports}, PortWidth:{port_width}, DataWidth:{data_width}. Too many streams for p*(pw/dw): {ports*(port_width/data_width)}")
    # get port data type
    if   port_width == 8:
        port_type = np.uint8
    elif port_width == 16:
        port_type = np.uint16
    elif port_width == 32:
        port_type = np.uint32
    elif port_width == 64:
        port_type = np.uint64
    else:
        raise TypeError
    # get stream data type
    if   0  < data_width <= 8:
        data_type = np.uint8
    elif 8  < data_width <= 16:
        data_type = np.uint16
    elif 16 < data_width <= 32:
        data_type = np.uint32
    elif 32 < data_width <= 64:
        data_type = np.uint64
    else:
        raise TypeError
    # check streams are a factor of the stream shape
    if not stream.shape[0]%streams == 0:
        raise ValueError
    # get the size of the binary streas out
    size = int(stream.shape[0]/streams)
    # binary stream out
    bin_out = np.zeros([ports,size], dtype=port_type)
    # copy stream to binary stream out
    for i in range(size):
        for j in range(streams):
            port_index = math.floor((j*data_width)/port_width)
            # print(stream[i*streams+j].bits_to_signed() & ((2**data_width)-1), data_type(stream[i*streams+j].bits_to_signed()))
            stream_val = data_type( stream[i*streams+j].bits_to_signed() & ((2**data_width)-1) )
            bin_out[port_index][i] |= port_type( stream_val  << (data_width*j)%port_width )
    # return the formatted stream
    return bin_out

def _fixed_point_stream_to_bin(stream, output_path, streams=1, port_width=64, ports=1):
    # get the formatted_stream
    bin_out = _fixed_point_stream_format(stream, streams=streams, port_width=port_width, ports=ports)
    # get the port type
    port_type = bin_out.dtype
    # save to binary file
    for i in range(ports):
        bin_out[i].astype(port_type).tofile(f"{output_path}_{i}.bin".format(i=i))

def _fixed_point_stream_to_dat(stream, output_path, streams=1, port_width=64, ports=1):
    # check it's only a 1D array
    assert len(stream.shape) == 1
    # check the stream is fixed-point
    # TODO
    # get width of fixed point data
    data_width = sum(stream[0].format)
    # check theres enough ports for the streams
    print(f"Streams:{streams}, Ports:{ports}, PortWidth:{port_width}, DataWidth:{data_width}, Maximum stream number: {ports*(port_width/data_width)}, Number of elements = {stream.shape[0]}")
    if streams > ports*(port_width/data_width):
        raise ValueError(f"Streams:{streams}, Ports:{ports}, PortWidth:{port_width}, DataWidth:{data_width}. Insufficient port_width, too many streams for p*(pw/dw): {ports*(port_width/data_width)}")
    # get port data type
    # if   port_width == 8:
    #     port_type = np.uint8
    # elif port_width == 16:
    #     port_type = np.uint16
    # elif port_width == 32:
    #     port_type = np.uint32
    # elif port_width == 64:
    #     port_type = np.uint64
    # else:
    #     raise TypeError
    if port_width not in [8, 16, 32, 64, 128, 256, 512]:
        raise TypeError ("port_width must be 8, 16, 32, 64, 128, 256, or 512")
    # get stream data type
    if   0  < data_width <= 8:
        data_type = np.uint8
    elif 8  < data_width <= 16:
        data_type = np.uint16
    elif 16 < data_width <= 32:
        data_type = np.uint32
    elif 32 < data_width <= 64:
        data_type = np.uint64
    else:
        raise TypeError ("data_width must be 8, 16, 32, or 64")
    # check streams are a factor of the stream shape
    if not stream.shape[0]%streams == 0:
        raise ValueError
    # get the size of the binary streas out, size is number of lines in .dat file
    size = int(stream.shape[0]/streams)
    # binary stream init
    bin_out = np.zeros([ports,size], dtype=object)
    # binary stream packing
    for i in range(size):
        for j in range(streams):
            port_index = math.floor((j*data_width)/port_width) # streams*data_width/port_width has to be < 1
            stream_val = int(data_type( stream[i*streams+j].bits_to_signed() & ((2**data_width)-1) ))
            # bin_out[port_index][i] |= port_type( stream_val  << (data_width*j)%port_width )
            bin_out[port_index][i] += int( stream_val  << (data_width*j)%port_width ) # replace |= for undetermined port width
    # return the formatted stream
    for i in range(ports):
        with open(f"{output_path}_{i}.dat", 'w') as f:
            f.write("\n".join([str(j) for j in bin_out[i]]))

    # def transform_featuremap(self, featuremap):
    #     # normalise
    #     #featuremap = self.normalise(featuremap) # TODO: remove
    #     # transform featuremap
    #     return np.moveaxis(featuremap, 1, -1)
    #     # TODO: handle 1D and 2D featuremaps

    # def save_featuremap(self, featuremap, output_path, parallel_streams=1, to_yaml=False, to_bin=False, to_csv=False, to_dat=False):
    #     # get feature map stream
    #     stream = self._convert_fixed_port_stream(featuremap.reshape(-1))
    #     # binary format
    #     if to_bin:
    #         self._fixed_point_stream_to_bin(stream, output_path, streams=parallel_streams)
    #     # dat format
    #     if to_dat:
    #         self._fixed_point_stream_to_dat(stream, output_path, streams=parallel_streams)
    #     # csv format
    #     if to_csv:
    #         pass

    # def save_featuremap_in_out(self, output_path, to_bin=False, to_csv=False, to_dat=False):
    #     # save input layer
    #     input_node = self.partition.input_node
    #     input_data = np.array( self.sess.run([input_node], { self.input_name : self.data } )[0] )
    #     input_data = self.transform_featuremap(input_data)
    #     input_streams = int(self.partition.layers[0].parameters.coarse_in)
    #     self.save_featuremap(input_data, os.path.join(output_path, onnx_helper._format_name(input_node)),
    #         parallel_streams=input_streams, to_yaml=False, to_bin=to_bin, to_csv=to_csv, to_dat=to_dat)
    #     # save output layer
    #     output_node = self.partition.output_node
    #     output_data = np.array( self.sess.run([output_node], { self.input_name : self.data } )[0] )
    #     output_data = self.transform_featuremap(output_data)
    #     output_streams = int(self.partition.layers[-1].parameters.coarse_out)
    #     self.save_featuremap(output_data, os.path.join(output_path, onnx_helper._format_name(output_node)),
    #         parallel_streams=output_streams, to_yaml=False, to_bin=to_bin, to_csv=to_csv, to_dat=to_dat)

    """
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    WEIGHTS
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    """

def _transform_weights(weights_raw, wr_factor, coarse_in, coarse_out, coarse_group, groups):
    # parameters
    num_filters  = int(weights_raw.shape[0]/(groups*coarse_out*wr_factor))
    num_channels = int(weights_raw.shape[1]/coarse_in)
    k_size_x       = weights_raw.shape[2]
    k_size_y       = weights_raw.shape[3]
    # correct output shape for weights
    weights = np.ndarray(
        shape=(
            wr_factor,
            coarse_group,
            coarse_in,
            coarse_out,
            int(groups/coarse_group),
            num_channels,
            num_filters,
            k_size_x,k_size_y),dtype=float,order='C')

    # transform weights raw shape
    for index,_ in np.ndenumerate(weights):
        weights[index] = weights_raw[
                  index[4]*coarse_group*num_filters*wr_factor*coarse_out+index[1]*num_filters*wr_factor*coarse_out+index[6]*wr_factor*coarse_out+index[0]*coarse_out+index[3],
                  index[5]*coarse_in+index[2],
                  index[7],
                  index[8]]
    # merge channel and filter dimensions
    weights = np.reshape(weights,[wr_factor,coarse_in*coarse_group,coarse_out,int(groups/coarse_group)*num_channels*num_filters,k_size_x,k_size_y])
    # return transformed weights
    return weights

def get_weights_convolution(weights_raw, layer, wr_factor=1):
    # transform parameters
    coarse_in    = layer.parameters.coarse_in
    coarse_out   = layer.parameters.coarse_out
    coarse_group = layer.parameters.coarse_group
    groups       = layer.parameters.groups
    # return transformed weights
    return _transform_weights(weights_raw,wr_factor,coarse_in,coarse_out,coarse_group,groups)

def get_weights_inner_product(weights_raw, layer, wr_factor=1):
    # get weights
    # weights_raw = onnx_helper.get_model_initializer(self.model, layer.weights_path)
    # transform parameters
    coarse_in   = layer.parameters.coarse_in
    coarse_out  = layer.parameters.coarse_out
    filters     = layer.parameters.filters
    channels    = layer.parameters.channels_in
    rows        = layer.parameters.rows_in
    cols        = layer.parameters.cols_in
    #reshape for transforming
    weights_raw = np.reshape(weights_raw,(filters*wr_factor,channels,rows,cols))
    weights_raw = np.rollaxis(weights_raw,1,4) #input filters need to be FINAL axis
    weights_raw = np.reshape(weights_raw,(filters*wr_factor,rows*cols*channels,1,1))
    # return transformed weights
    return _transform_weights(weights_raw,wr_factor,coarse_in,coarse_out,1,1)

def _transform_biases(biases_raw, filters, coarse_out, wr_factor=1):
    # parameters
    num_filters  = biases_raw.shape[0]//(coarse_out*wr_factor)
    biases = np.ndarray(
        shape=(
            wr_factor,
            coarse_out,
            num_filters
            ), dtype=float, order='C')#order is row major

    # transform biases raw shape
    for index,_ in np.ndenumerate(biases):
        biases[index] = biases_raw[coarse_out*wr_factor*index[2]+index[1]+index[0]]

    # return transformed biases
    return biases

def get_biases(biases_raw, layer, wr_factor=1):
    # transform parameters
    coarse_out  = layer.parameters.coarse_out
    filters = layer.parameters.filters
    # return transformed biases
    return _transform_biases(biases_raw, filters, coarse_out, wr_factor)


    # def save_weights_layer(self,layer,wr_factor=1,output_path=None,to_yaml=False,to_bin=False,to_csv=False,to_dat=False):
    #     # get transformed weights
    #     if layer_enum.from_proto_layer_type(layer.type) == layer_enum.LAYER_TYPE.Convolution:
    #         transformed_weights = self.get_weights_convolution(layer, wr_factor=wr_factor)
    #     elif layer_enum.from_proto_layer_type(layer.type) == layer_enum.LAYER_TYPE.InnerProduct:
    #         transformed_weights = self.get_weights_inner_product(layer, wr_factor=wr_factor)
    #     else:
    #         raise TypeError
    #     # save weights
    #     if output_path:
    #         # csv format
    #         if to_csv:
    #             # iterate over weights reloading factor
    #             for weights_reloading_index in range(wr_factor):
    #                 # get filepath name
    #                 filepath = f'{output_path}_{weights_reloading_index}.csv'
    #                 # save to csv file
    #                 with open(filepath, 'w') as f:
    #                     f.write(array_init(transformed_weights[weights_reloading_index]))
    #         # get the bitwidth for the weights
    #         bitwidth = layer.parameters.weight_width
    #         # flatten the weights for binary and data formats
    #         weights_stream =  self._convert_fixed_port_stream(transformed_weights.reshape(-1), total_width=bitwidth, int_width=bitwidth//2)
    #         # bin format
    #         if to_bin:
    #             self._fixed_point_stream_to_bin(weights_stream, output_path=output_path, streams=1, port_width=64, ports=1)
    #         # dat format
    #         if to_dat:
    #             self._fixed_point_stream_to_dat(weights_stream, output_path=output_path, streams=1, port_width=64, ports=1)
    #     # return transformed weights
    #     return transformed_weights

    # def save_weights_partition(self,output_path,to_yaml=False,to_bin=False,to_csv=False,to_dat=False):

    #     def _fix_identifier(name):
    #         if name[0].isdigit():
    #             return "n" + name
    #         else:
    #             return name
    #     weights = {}
    #     # iterate over layers in network
    #     for layer in self.partition.layers:
    #         layer_type_str = str(fpgaconvnet_pb2.layer.layer_type.Name(layer.type)) # REQUIRED EDIT
    #         layer_name = gen_layer_name(layer) # REQUIRED EDIT
    #         # skip weights outside of partition
    #         if layer_enum.from_proto_layer_type(layer.type) in [ layer_enum.LAYER_TYPE.Convolution, layer_enum.LAYER_TYPE.InnerProduct ]:
    #             # get weights reloading factor
    #             if layer.name == self.partition.weights_reloading_layer:
    #                 wr_factor = self.partition.weights_reloading_factor
    #             else:
    #                 wr_factor = 1
    #             # get output path
    #             output_path_layer = None
    #             if output_path:
    #                 layer_identifier = _fix_identifier(layer.name)
    #                 output_path_layer = os.path.join(output_path,f"{layer_identifier}_weights")
    #             # get layer info
    #             weights[layer.name] = self.save_weights_layer(layer,wr_factor=wr_factor,
    #                     output_path=output_path_layer,to_bin=to_bin,to_csv=to_csv,to_dat=to_dat)
    #     # yaml format
    #     if to_yaml:
    #         # save data as .dat files
    #         print("YAML file usage deprecated, creating .dat files instead")
    #         for layer in weights:
    #             weight_list = weights[layer].reshape(-1).tolist()
    #             with open(os.path.join(output_path,layer+".dat"), 'w') as f:
    #                 f.write("\n".join([str(i) for i in weight_list]))

    #     return weights
