fork_template = """
{indent}fork<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_COARSE,
{_if}       {NAME}_KERNEL_SIZE_X > 1 || {NAME}_KERNEL_SIZE_Y > 1
{_if_conv}  {NAME}_KERNEL_SIZE_X > 1 || {LAYER_NAME}_KERNEL_SIZE_Y > 1 || {LAYER_NAME}_STRIDE_X > 1 || {NAME}_STRIDE_Y > 1
{indent}    {NAME}_KERNEL_SIZE_X,
{indent}    {NAME}_KERNEL_SIZE_Y,
#endif
{indent}    {fork_t}
{indent}>({input_stream},{output_stream});
"""

def gen_fork_module(name,input_stream,output_stream,
        fork_t="data_t",indent=0):
    return fork_template.format(
        LAYER_NAME      =name.upper(),
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        fork_t          =fork_t,
        _if             ="#if",
        _if_conv        ="//",
        indent          =" "*indent
    )
    
def gen_conv_fork_module(layer_name, name,input_stream,output_stream,
        fork_t="data_t",indent=0):
    return fork_template.format(
        LAYER_NAME      =layer_name.upper(),
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        fork_t          =fork_t,
        _if             ="//",
        _if_conv        ="#if",
        indent          =" "*indent
    )

# from dataclasses import dataclass
# from typing import Union

# from fpgaconvnet.hls.generate.modules import Module
# from fpgaconvnet.models.modules import Fork

#@dataclass
#class GenerateFork(GenerateModule):
#    batch_size: Union[int,str]
#    rows: Union[int,str]
#    cols: Union[int,str]
#    channels: Union[int,str]
#    coarse: Union[int,str]
#    kernel_size_x: Union[int,str]
#    kernel_size_y: Union[int,str]
#    fork_t: str = "data_t"
#    name: str = "fork"

#    def __post_init__(self):
#        # create reference fork module from models
#        self.ref = Fork(
#                self.rows,
#                self.cols,
#                self.channels,
#                [
#                    self.kernel_size_x,
#                    self.kernel_size_y
#                ],
#                self.coarse
#        )

#    def create_module(self, input_stream, output_stream, indent=0):
#        indent_tabs = "\t"*indent
#        return f"""
#{indent_tabs}fork<
#{indent_tabs}    {self.batch_size},
#{indent_tabs}    {self.rows},
#{indent_tabs}    {self.cols},
#{indent_tabs}    {self.channels},
#{indent_tabs}    {self.coarse},
##if {self.kernel_size_x} > 1 || {self.kernel_size_y} > 1
#{indent_tabs}    {self.kernel_size_x},
#{indent_tabs}    {self.kernel_size_y},
##endif
#{indent_tabs}    {self.fork_t}
#{indent_tabs}>({input_stream},{output_stream});
#        """

