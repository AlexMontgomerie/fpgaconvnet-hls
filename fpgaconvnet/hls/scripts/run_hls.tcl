# load getopt script
source $::env(FPGACONVNET_HLS)/scripts/tcl_getopt.tcl

# get fpgaconvnet root folder
set fpgaconvnet_root $::env(FPGACONVNET_HLS)

# get input arguments
set hls_arg [ lindex $argv 2 ]

# get arguments (arg)   (variable)  (defaults)
getopt $hls_arg -num    test_num    ""
getopt $hls_arg -type   test_type   ""
getopt $hls_arg -name   name        ""
getopt $hls_arg -fpga   fpga        "xc7z020clg484-1"
#getopt $hls_arg -fpga   fpga        "xc7z045ffg900-2"
getopt $hls_arg -clk    clk_period  "5"

# get type of test
set module_flag     [ getopt $hls_arg -module   ]
set layer_flag      [ getopt $hls_arg -layer    ]
set network_flag    [ getopt $hls_arg -network  ]
set reset_flag      [ getopt $hls_arg -reset    ]
set fast_flag       [ getopt $hls_arg -fast     ]

# check a name is given
if { ![ info exists name ] } {
    puts "ERROR: need to specify project name"
    exit
}

# TODO: print info about run
puts "### FPGACONVNET HLS RUN ###           "
puts " ( project variables )                "
puts "  - config number = ${test_num}       "
puts "  - run type      = ${test_type}      "
puts "  - project name  = ${name}           "
puts "  - fpga part     = ${fpga}           "
puts " ( project flags )                    "
puts "  - module flag   = ${module_flag}    "
puts "  - layer flag    = ${layer_flag}     "
puts "  - network flag  = ${network_flag}   "
puts "  - reset flag    = ${reset_flag}     "
puts "###########################           "

# open project
if { $reset_flag == 1 } {
    open_project -reset ${name}_hls_prj
} else {
    open_project ${name}_hls_prj
}

# set top name
if { $layer_flag == 1 } {
    set_top ${name}_layer_top
} elseif { $network_flag == 1 } {
    set_top fpgaconvnet_ip
} else {
    set_top ${name}_top
}
# add files
if          { $module_flag == 1     } {
    add_files src/${name}.cpp               -cflags "-Wtautological-compare -Wno-parentheses-equality -std=c++11 -I../../../include -I../../../src -I./tb"
    add_files -tb tb/${name}_tb.cpp         -cflags "-Wtautological-compare -Wno-parentheses-equality -std=c++11 -I../../../include -I../../../src -I./tb"
} elseif    { $layer_flag == 1      } {
    add_files src/${name}_layer.cpp         -cflags "-Wtautological-compare -Wno-parentheses-equality -std=c++11 -I../../../include -I./tb -I./include"
    #add_files src/${name}_layer_${name}.cpp -cflags "-Wtautological-compare -Wno-parentheses-equality -std=c++11 -I../../../include -I./tb -I./include"
    add_files src/${name}_layer_top.cpp     -cflags "-Wtautological-compare -Wno-parentheses-equality -std=c++11 -I../../../include -I./tb -I./include -I./data"
    add_files -tb tb/${name}_layer_tb.cpp   -cflags "-Wtautological-compare -Wno-parentheses-equality -std=c++11 -I../../../include -I./tb -I./include"
} elseif    { $network_flag == 1    } {
    add_files [ glob src/*.cpp ]            -cflags "-Wtautological-compare -Wno-parentheses-equality -std=c++11 -I${fpgaconvnet_root}/include -I./tb -I./include -I./data"
    add_files -tb tb/${name}_tb.cpp         -cflags "-Wtautological-compare -Wno-parentheses-equality -std=c++11 -I${fpgaconvnet_root}/include -I./tb -I./include -lyaml-cpp"
}

# open the solution
if { [ info exists test_num ] } {
    open_solution -reset "solution${test_num}"
} else {
    open_solution -reset "solution"
}

# set fpga part
set_part $fpga -tool vivado

# increase fifo depth
config_dataflow -default_channel fifo -fifo_depth 2
config_dataflow -strict_mode warning

#set_directive_interface -mode m_axi -depth 1 -offset slave -bundle out_0 "mem_write_top" out_hw
#set_directive_interface -mode m_axi -depth 1 -offset slave -bundle out_1 "mem_write_top" out_hw_1

# set clock period
create_clock -period $clk_period -name default

# define vivado hls procedures
#proc csim   {} { csim_design -mflags "-j 8" }
proc csim   {} { csim_design }
proc csynth {} { csynth_design }
proc cosim  {} { cosim_design -rtl verilog -trace_level all }
proc export {} { export_design -flow impl -rtl verilog -format ip_catalog }
#if { $fast_flag == 1 } {
#    proc export {} { export_design -rtl verilog -format ip_catalog }
#} else {
#    proc export {} { export_design -flow impl -rtl verilog -format ip_catalog }
#}
# run hls tool
if { $test_type == "sim" } {

    csim
    exit

} elseif { $test_type == "synth" } {

    if { $fast_flag == 1 } {
        csynth
    } else {
        csim
        csynth
    }
    exit

} elseif { $test_type == "cosim" } {

    if { $fast_flag == 1 } {
        csynth
        cosim
    } else {
        csim
        csynth
        cosim
    }
    exit

} elseif { $test_type == "impl" } {

    csynth
    export
    exit

} elseif { $test_type == "all" } {

    csim
    csynth
    cosim
    export
    exit

} else      {

    exit

}

exit
