# load getopt script
source ../../../fpgaconvnet/hls/scripts/hls/tcl_getopt.tcl

# get fpgaconvnet root folder
set fpgaconvnet_hardware_path ../../../fpgaconvnet/hls/hardware

# get input arguments
set hls_arg [ lindex $argv 2 ]

# get arguments (arg)   (variable)  (defaults)
getopt $hls_arg -num    test_num    ""
getopt $hls_arg -type   test_type   "csim"
getopt $hls_arg -name   name        ""
getopt $hls_arg -fpga   fpga        "xc7z020clg484-1"
getopt $hls_arg -clk    clk_period  "5"

# check a name is given
if { ![ info exists name ] } {
    puts "ERROR: need to specify project name"
    exit
}

# check a test number is given
if { ![ info exists test_num ] } {
    puts "ERROR: need to specify the test number"
    exit
}

puts "### module test ${name} ###           "
puts "  - config number = ${test_num}       "
puts "  - run type      = ${test_type}      "
puts "  - project name  = ${name}           "
puts "  - fpga part     = ${fpga}           "
puts "  - clock period  = ${clk_period}     "
puts "###########################           "

# open project
open_project -reset ${name}_hls_prj

# set top name
set_top ${name}_top

# compiler flags
set compiler_flags "-std=c++11 -fexceptions -I../../../src -I../../../include\
   -I${fpgaconvnet_hardware_path} -I${fpgaconvnet_hardware_path}/hlslib/include -I./tb"

# add files
add_files ./src/${name}.cpp -cflags "${compiler_flags}"
add_files -tb ./tb/${name}_tb.cpp -cflags "${compiler_flags}"

# open the solution
open_solution -reset "solution${test_num}"

# set fpga part
set_part $fpga -tool vivado

# increase fifo depth
config_dataflow -default_channel fifo -fifo_depth 2
config_dataflow -strict_mode warning

# set clock period
create_clock -period $clk_period -name default

# run the hls tool
if { $test_type == "sim" } {

    csim_design
    exit

} elseif { $test_type == "synth" } {

    # csim_design
    csynth_design
    exit

} elseif { $test_type == "cosim" } {

    csim_design
    csynth_design
    cosim_design -rtl verilog -trace_level all
    exit

} elseif { $test_type == "impl" } {

    csynth_design
    export_design -flow impl -rtl verilog -format ip_catalog
    exit

} elseif { $test_type == "all" } {

    csim_design
    csynth_design
    cosim_design -rtl verilog -trace_level all
    export_design -flow impl -rtl verilog -format ip_catalog
    exit

} else {

    puts "ERROR: need to specify valid test (csim, synth, cosim, impl, all)"
    exit

}

exit
