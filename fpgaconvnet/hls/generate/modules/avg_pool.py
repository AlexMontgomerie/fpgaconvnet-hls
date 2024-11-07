avg_pool_template="""
{indent}avg_pool<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_KERNEL_SIZE_X,
{indent}    {NAME}_KERNEL_SIZE_Y, 
{indent}    {avg_pool_t}
{indent}>({input_stream},{output_stream});
"""

def gen_avg_pool_module(name,input_stream,output_stream,
        avg_pool_t="data_t",indent=0):
    return avg_pool_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        avg_pool_t      =avg_pool_t,
        indent          =" "*indent
    )
    