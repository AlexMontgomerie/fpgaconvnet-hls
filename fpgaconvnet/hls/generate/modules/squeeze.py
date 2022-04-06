squeeze_template = """
{indent}squeeze<
#if ( {NAME}_BATCH_SIZE*{NAME}_COLS*{NAME}_ROWS*{NAME}_CHANNELS_PER_COARSE_IN > 1 ) && ( {NAME}_BATCH_SIZE*{NAME}_COLS*{NAME}_ROWS*{NAME}_CHANNELS_PER_COARSE_OUT > 1 )
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
#endif
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_COARSE_IN,
{indent}    {NAME}_COARSE_OUT,
#if {NAME}_BATCH_SIZE*{NAME}_COLS*{NAME}_ROWS*{NAME}_CHANNELS_PER_COARSE_IN > 1
{indent}    {NAME}_CHANNELS_PER_COARSE_IN,
#endif
{indent}    {NAME}_BUFFER_SIZE,
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
