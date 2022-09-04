# get the root directory
variable fpgaconvnet_root [file dirname [file dirname [file dirname [file normalize [info script]]]]]

# load getopt script
source ${fpgaconvnet_root}/scripts/hls/tcl_getopt.tcl

# get input arguments
set hls_arg [ lindex $argv 2 ]

# get arguments (arg)   (variable)      (defaults)
getopt $hls_arg -prj    project_path    ""
getopt $hls_arg -fpga   fpga            "xc7z045ffg900-2"
getopt $hls_arg -clk    clk_period      "5"

puts "project: ${project_path}"

# default cflags
set default_cflags "-std=c++11 -fexceptions -I${project_path}/include -I${project_path}/data \
    -I${fpgaconvnet_root}/hardware -I${fpgaconvnet_root}/hardware/hlslib/include"

# create open project
open_project ${project_path}

# set top function
set_top fpgaconvnet_ip

# add files to the project
add_files [ glob ${project_path}/src/*.cpp ] -cflags "${default_cflags}"

# add testbench file to the project
add_files -tb [ glob ${project_path}/tb/*.cpp ] -cflags "${default_cflags}"

# add any data files
add_files -tb [ glob ${project_path}/data/*.dat ]

# create the solution
open_solution "solution"

# set FPGA part
set_part $fpga -tool vivado

# set clock period
create_clock -period $clk_period -name default

# increase fifo depth
config_dataflow -default_channel fifo -fifo_depth 2
config_dataflow -strict_mode warning

exit
