# get the root directory
variable fpgaconvnet_root [file dirname [file dirname [file dirname [file normalize [info script]]]]]

# load getopt script
source ${fpgaconvnet_root}/scripts/tcl_getopt.tcl

# get input arguments
set hls_arg [ lindex $argv 2 ]

# get arguments (arg)   (variable)      (defaults)
getopt $hls_arg -name   name            ""

# open project
open_project ${name}

# open solution
open_solution "solution"

# run export design
export_design -rtl verilog -format ip_catalog

exit
