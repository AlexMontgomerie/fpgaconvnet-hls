accum_template = """
{indent}accum<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_FILTERS,
{indent}    {NAME}_GROUPS,
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
