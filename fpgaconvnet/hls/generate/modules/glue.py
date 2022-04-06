glue_template="""
{indent}glue<
#if {NAME}_BATCH_SIZE*{NAME}_ROWS*{NAME}_COLS*{NAME}_FILTERS_PER_COARSE > 1
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_FILTERS,
#endif
{indent}    {NAME}_COARSE_IN,
{indent}    {NAME}_COARSE_OUT,
{indent}    {NAME}_COARSE_GROUP,
{indent}    {acc_t},
{indent}    {data_t}
{indent}>({input_stream},{output_stream});
"""

def gen_glue_module(name,input_stream,output_stream,
        acc_t="acc_t",data_t="data_t",indent=0):
    return glue_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        acc_t           =acc_t,
        data_t          =data_t,
        indent          =" "*indent
    )
