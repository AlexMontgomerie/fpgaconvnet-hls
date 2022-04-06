pool_template="""
{indent}pool<
#if {NAME}_BATCH_SIZE*{NAME}_ROWS*{NAME}_COLS*{NAME}_CHANNELS > 1
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
#endif
{indent}    {NAME}_KERNEL_SIZE_X,
{indent}    {NAME}_KERNEL_SIZE_Y,
{indent}    {pool_t}
{indent}>({input_stream},{output_stream});
"""

def gen_pool_module(name,input_stream,output_stream,
        pool_t="data_t",indent=0):
    return pool_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        pool_t          =pool_t,
        indent          =" "*indent
    )
