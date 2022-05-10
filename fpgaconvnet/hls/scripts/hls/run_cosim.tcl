# get the root directory
variable fpgaconvnet_root [file dirname [file dirname [file dirname [file normalize [info script]]]]]

# load getopt script
source ${fpgaconvnet_root}/scripts/hls/tcl_getopt.tcl

# get input arguments
set hls_arg [ lindex $argv 2 ]

# get arguments
getopt $hls_arg -prj project_path

# open project
open_project ${project_path}

# add any data files
add_files -tb [ glob ${project_path}/data/*.dat ]

# open solution
open_solution "solution"

# run co-simulation
cosim_design -rtl verilog -trace_level all

exit
