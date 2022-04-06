from fpgaconvnet.models.modules import Fork

# def gen_fork_module(name,input_stream,output_stream,
#         fork_t="data_t",indent=0):
#     return fork_template.format(
#         NAME            =name.upper(),
#         input_stream    =input_stream,
#         output_stream   =output_stream,
#         fork_t          =fork_t,
#         indent          =" "*indent,
#     )

@dataclass
class GenerateFork:
    batch_size: Union[int,str]
    rows: Union[int,str]
    cols: Union[int,str]
    channels: Union[int,str]
    coarse: Union[int,str]
    kernel_size_x: Union[int,str]
    kernel_size_y: Union[int,str]
    fork_t: str = "data_t"

    def __post_init__(self):
        # create reference fork module from models
        self.ref = Fork(
                self.rows,
                self.cols,
                self.channels,
                [
                    self.kernel_size_x,
                    self.kernel_size_y
                ],
                self.coarse
        )

    def create_module(self, input_stream, output_stream, indent=0):
        return f"""
{indent}fork<
{indent}    {self.batch_size},
{indent}    {self.rows},
{indent}    {self.cols},
{indent}    {self.channels},
{indent}    {self.coarse},
#if {self.kernel_size_x} > 1 || {self.kernel_size_y} > 1
{indent}    {self.kernel_size_x},
{indent}    {self.kernel_size_y},
#endif
{indent}    {self.fork_t}
{indent}>({input_stream},{output_stream});
        """

    def create_top_project(self, project_path, input_data):

        # generate reference output data

        pass
