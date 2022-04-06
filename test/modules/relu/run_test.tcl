open_project -reset relu_test_prj

if { $argc < 3 } {

    puts "ERROR: need to specify test"
    exit

} else {

    set TEST [lindex $argv 2]

    exec python3 gen_relu.py -c config/config_$TEST.json

    puts "TEST $TEST chosen"
    set_top relu_top
    add_files src/relu.cpp -cflags "-I../../include -I./tb"
    add_files -tb tb/relu_tb.cpp -cflags "-I../../include -I./tb -lyaml-cpp"

    open_solution -reset "solution1"
    set_part {xc7z020clg484-1} -tool vivado
    create_clock -period 20 -name default

    if { [lindex $argv 3] == "sim" } {

        csim_design -compiler gcc
        exit

    } elseif { [lindex $argv 3] == "synth" } {

        csim_design -compiler gcc
        csynth_design
        exit

    } elseif { [lindex $argv 3] == "cosim" } {

        csim_design -compiler gcc
        csynth_design
        cosim_design
        exit

    } else {

        csim_design -compiler gcc
        csynth_design
        cosim_design
        export_design -flow syn -rtl verilog -format ip_catalog
        exit

    }

}

exit
