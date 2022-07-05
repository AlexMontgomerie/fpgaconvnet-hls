conv_template = """
{indent}conv<
#if {NAME}_BATCH_SIZE*{NAME}_ROWS*{NAME}_COLS > 1
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
#endif
{indent}    {NAME}_CHANNELS,
#if {NAME}_FILTERS > 1
{indent}    {NAME}_FILTERS,
#endif
{indent}    {NAME}_GROUPS,
#if {NAME}_CHANNELS_PER_GROUP > 1
{indent}    {NAME}_CHANNELS_PER_GROUP,
#endif
#if {NAME}_FILTERS > 1
{indent}    {NAME}_FILTERS_PER_GROUP,
#endif
#if ({NAME}_KERNEL_SIZE_X > 1) || ({NAME}_KERNEL_SIZE_Y > 1)
{indent}    {NAME}_FINE,
{indent}    {NAME}_KERNEL_SIZE_X,
{indent}    {NAME}_KERNEL_SIZE_Y,
#endif
{indent}    {data_t},
{indent}    {weight_t},
{indent}    {acc_t}
#if ({NAME}_FILTERS == 1) && ({NAME}_CHANNELS_PER_GROUP == 1)
{indent}    ,{acc_t}
#endif
{indent}>({input_stream},{weights_stream},{output_stream});
"""

def gen_conv_module(name,input_stream,weights_stream,output_stream,
        data_t="data_t",weight_t="weight_t",acc_t="acc_t",indent=0):
    return conv_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        weights_stream  =weights_stream,
        output_stream   =output_stream,
        data_t          =data_t,
        weight_t        =weight_t,
        acc_t           =acc_t,
        indent          =" "*indent
    )
