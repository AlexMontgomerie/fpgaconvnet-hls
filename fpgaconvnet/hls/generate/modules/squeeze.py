squeeze_template = """
#if ( {NAME}_BATCH_SIZE*{NAME}_COLS*{NAME}_ROWS > 1 )
{indent}squeeze_spatial<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
#else
{indent}squeeze<
#endif
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_COARSE_IN,
{indent}    {NAME}_COARSE_OUT,
#if {NAME}_CHANNELS_PER_COARSE_IN > 1
{indent}    {NAME}_CHANNELS_PER_COARSE_IN,
{indent}    {NAME}_CHANNELS_PER_COARSE_IN,
#endif
#if {NAME}_CHANNELS_PER_COARSE_OUT > 1
{indent}    {NAME}_CHANNELS_PER_COARSE_OUT,
#endif
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
