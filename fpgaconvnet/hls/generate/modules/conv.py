conv_template = """
{indent}conv<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_FILTERS,
{indent}    {NAME}_GROUPS,
{_if} ({NAME}_KERNEL_SIZE_X > 1) || ({NAME}_KERNEL_SIZE_Y > 1)
{_if_conv} ({NAME}_KERNEL_SIZE_X > 1) || ({NAME}_KERNEL_SIZE_Y > 1) || ({LAYER_NAME}_STRIDE_X > 1) || ({LAYER_NAME}_STRIDE_Y > 1)
{indent}    {NAME}_FINE,
{indent}    {NAME}_KERNEL_SIZE_X,
{indent}    {NAME}_KERNEL_SIZE_Y,
#endif
{indent}    {data_t},
{indent}    {weight_t},
{indent}    {acc_t}
{indent}>({input_stream},{weights_stream},{output_stream});
"""

def gen_conv_module(name,input_stream,weights_stream,output_stream,
        data_t="data_t",weight_t="weight_t",acc_t="acc_t",indent=0):
    return conv_template.format(
        LAYER_NAME      =name.upper(),
        NAME            =name.upper(),
        input_stream    =input_stream,
        weights_stream  =weights_stream,
        output_stream   =output_stream,
        data_t          =data_t,
        weight_t        =weight_t,
        acc_t           =acc_t,
        _if             ="#if",
        _if_conv        ="//",
        indent          =" "*indent
    )

def gen_convolution_conv_module(layer_name,name,input_stream,weights_stream,output_stream,
        data_t="data_t",weight_t="weight_t",acc_t="acc_t",indent=0):
    return conv_template.format(
        LAYER_NAME      =layer_name.upper(),
        NAME            =name.upper(),
        input_stream    =input_stream,
        weights_stream  =weights_stream,
        output_stream   =output_stream,
        data_t          =data_t,
        weight_t        =weight_t,
        acc_t           =acc_t,
        _if             ="//",
        _if_conv        ="#if",
        indent          =" "*indent
    )

