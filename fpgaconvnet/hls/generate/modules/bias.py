bias_template="""
{indent}bias<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_FILTERS,
{indent}    {data_t},
{indent}    {biases_t}
{indent}>({input_stream},{biases},{output_stream});
"""

def gen_bias_module(name, input_stream, biases, output_stream,
        data_t="data_t",biases_t="data_t",indent=0):
    return bias_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        biases          =biases,
        output_stream   =output_stream,
        data_t          =data_t,
        biases_t        =biases_t,
        indent          =" "*indent
    )
