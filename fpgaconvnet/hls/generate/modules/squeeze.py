squeeze_template = """
{indent}squeeze<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_COARSE_IN,
{indent}    {NAME}_COARSE_OUT,
{indent}    {squeeze_t}
{indent}>({input_stream},{output_stream});
"""

def gen_squeeze_module(name,input_stream,output_stream,
        squeeze_t="data_t",indent=0):
    return squeeze_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        squeeze_t       =squeeze_t,
        indent          =" "*indent
    )
