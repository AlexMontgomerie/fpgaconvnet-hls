global_pool_template="""
{indent}global_pool<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {global_pool_t}
{indent}>({input_stream},{output_stream});
"""

def gen_global_pool_module(name,input_stream,output_stream,
        global_pool_t="data_t",indent=0):
    return global_pool_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        global_pool_t   =global_pool_t,
        indent          =" "*indent
    )