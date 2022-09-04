sliding_window_template = """
{indent}sliding_window<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_PAD_TOP,
{indent}    {NAME}_PAD_RIGHT,
{indent}    {NAME}_PAD_BOTTOM,
{indent}    {NAME}_PAD_LEFT,
{indent}    {NAME}_STRIDE_X,
{indent}    {NAME}_STRIDE_Y,
{indent}    {NAME}_KERNEL_SIZE_X,
{indent}    {NAME}_KERNEL_SIZE_Y,
{indent}    {sliding_window_t}
{indent}>({input_stream},{output_stream});
"""
def gen_sliding_window_module(name,input_stream,output_stream,
        sliding_window_t="data_t",indent=0):
    return sliding_window_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        sliding_window_t=sliding_window_t,
        indent          =" "*indent
    )

