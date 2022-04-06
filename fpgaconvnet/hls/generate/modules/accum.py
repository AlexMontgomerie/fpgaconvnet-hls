accum_template = """
{indent}accum<
#if {NAME}_BATCH_SIZE*{NAME}_ROWS*{NAME}_COLS*{NAME}_GROUPS > 1
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
#endif
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_FILTERS,
#if {NAME}_FILTERS_PER_GROUP > 1
{indent}    {NAME}_FILTERS_PER_GROUP,
#endif
#if {NAME}_BATCH_SIZE*{NAME}_ROWS*{NAME}_COLS*{NAME}_GROUPS > 1
{indent}    {NAME}_GROUPS,
#endif
{indent}    {accum_t}
{indent}>({input_stream},{output_stream});
"""

def gen_accum_module(name,input_stream,output_stream,
        accum_t="data_t",indent=0):
    return accum_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        accum_t         =accum_t,
        indent          =" "*indent
    )
