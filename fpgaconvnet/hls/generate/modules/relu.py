relu_template = """
{indent}relu<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {relu_t}
{indent}>({input_stream},{output_stream});
"""

def gen_relu_module(name,input_stream,output_stream,
        relu_t="data_t",indent=0):
    return relu_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        relu_t          =relu_t,
        indent          =" "*indent
    )

