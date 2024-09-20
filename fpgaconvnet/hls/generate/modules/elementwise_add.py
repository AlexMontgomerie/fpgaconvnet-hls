elementwise_add_template = """
{indent}elementwise_add<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {elementwise_add_t}
{indent}>({input_stream_1},{input_stream_2},{output_stream});
"""

def gen_elementwise_add_module(name,input_stream_1,input_stream_2,output_stream,
        elementwise_add_t="data_t",indent=0):
    return elementwise_add_template.format(
        NAME                =name.upper(),
        input_stream_1      =input_stream_1,
        input_stream_2      =input_stream_2,
        output_stream       =output_stream,
        elementwise_add_t   =elementwise_add_t,
        indent              =" "*indent
    )