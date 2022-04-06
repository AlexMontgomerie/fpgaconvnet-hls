################################################################################
# vivado -mode batch -notrace -source gen_hw.tcl -tclargs argv[0]
#  ( argv 1 : network name          )
#  ( argv 2 : fpga part name        )
#  ( argv 3 : fpga board part       )
#  ( argv 4 : board frequency       )
#  ( argv 5 : port width            )
#  ( argv 6 : weights reloading on  )
################################################################################
if { $argc < 3 } {

    puts "ERROR: need to specify arguments"
    exit

} else {

    # get variables
    set NET         [ lindex $argv 0 ]
    set PART        [ lindex $argv 1 ]
    set BOARD       [ lindex $argv 2 ]
    set FREQ        [ lindex $argv 3 ]
    set PORT_WIDTH  [ lindex $argv 4 ]
    set WR_ON       [ lindex $argv 5 ]

    set PATH $::env(PWD)
    set dir_name "${NET}_hw_prj"

    # Create Vivado Project
    create_project -force project_1 $PATH/$dir_name/project_1 -part $PART
    # Set FPGA device/board
    set_property board_part $BOARD [current_project]

    # Add CNN IP core from HLS to the current IP Catalog
    set ip_name "${NET}_hls_prj/solution"
    set_property  ip_repo_paths  $PATH/$ip_name/impl/ip [current_project]
    update_ip_catalog
    update_ip_catalog

    # Generate the base block design for system implementation
    source $::env(FPGACONVNET_HLS)/scripts/gen_bd.tcl
    generate_hardware $BOARD $WR_ON $PORT_WIDTH $FREQ 0 0

    # Reset output products
    reset_target all [get_files  $PATH/$dir_name/project_1/project_1.srcs/sources_1/bd/design_1/design_1.bd]
    export_ip_user_files -of_objects  [get_files  $PATH/$dir_name/project_1/project_1.srcs/sources_1/bd/design_1/design_1.bd] -sync -no_script -force -quiet

    # Geneate output products
    set_property synth_checkpoint_mode None [get_files  $PATH/$dir_name/project_1/project_1.srcs/sources_1/bd/design_1/design_1.bd]
    generate_target all [get_files  $PATH/$dir_name/project_1/project_1.srcs/sources_1/bd/design_1/design_1.bd]
    export_ip_user_files -of_objects [get_files $PATH/$dir_name/project_1/project_1.srcs/sources_1/bd/design_1/design_1.bd] -no_script -sync -force -quiet
    export_simulation -of_objects [get_files $PATH/$dir_name/project_1/project_1.srcs/sources_1/bd/design_1/design_1.bd] -directory $PATH/$dir_name/project_1/project_1.ip_user_files/sim_scripts -ip_user_files_dir $PATH/$dir_name/project_1/project_1.ip_user_files -ipstatic_source_dir $PATH/$dir_name/project_1/project_1.ip_user_files/ipstatic -lib_map_path [list {modelsim=$PATH/$dir_name/project_1/project_1.cache/compile_simlib/modelsim} {questa=$PATH/$dir_name/project_1/project_1.cache/compile_simlib/questa} {ies=$PATH/$dir_name/project_1/project_1.cache/compile_simlib/ies} {xcelium=$PATH/$dir_name/project_1/project_1.cache/compile_simlib/xcelium} {vcs=$PATH/$dir_name/project_1/project_1.cache/compile_simlib/vcs} {riviera=$PATH/$dir_name/project_1/project_1.cache/compile_simlib/riviera}] -use_ip_compiled_libs -force -quiet

    # Create HDL wrapper
    make_wrapper -files [get_files $PATH/$dir_name/project_1/project_1.srcs/sources_1/bd/design_1/design_1.bd] -top
    add_files -norecurse $PATH/$dir_name/project_1/project_1.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v
    update_compile_order -fileset sources_1

    # Run Synthesis and Implementation (Place & Route)
    launch_runs impl_1 -to_step write_bitstream -jobs 8 -verbose
    wait_on_run impl_1 -verbose

    # Generate Bitstream in BIN format
    write_cfgmem -format BIN -interface SMAPx32 -disablebitswap -loadbit "up 0 $PATH/$dir_name/project_1/project_1.runs/impl_1/design_1_wrapper.bit" $PATH/$dir_name/project_1/design_1_wrapper.bin -force

    # Export Hardware
    #write_hw_platform -fixed -force  -include_bit -file $PATH/$dir_name/project_1/design_1_wrapper.xsa


# Export Harsdware (include Bitstream)
#file mkdir $PATH/$dir_name/project_1/project_1.sdk
#file copy -force $PATH/$dir_name/project_1/project_1.runs/impl_1/design_1_wrapper.sysdef $PATH/$dir_name/project_1/project_1.sdk/design_1_wrapper.hdf

# Launch Xilinx SDK
#launch_sdk -workspace $PATH/$dir_name/project_1/project_1.sdk -hwspec $PATH/$dir_name/project_1/project_1.sdk/design_1_wrapper.hdf

#  # Open Hardware Manager and auto-connect board
#  open_hw
#  connect_hw_server
#  open_hw_target
#  set_property PROGRAM.FILE {$PATH/$dir_name/project_1/project_1.runs/impl_1/design_1_wrapper.bit} [get_hw_devices xc7z020_1]
#  current_hw_device [get_hw_devices xc7z020_1]
#  refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xc7z020_1] 0]
#
#  # Program device
#  set_property PROBES.FILE {} [get_hw_devices xc7z020_1]
#  set_property FULL_PROBES.FILE {} [get_hw_devices xc7z020_1]
#  set_property PROGRAM.FILE {$PATH/$dir_name/project_1/project_1.runs/impl_1/design_1_wrapper.bit} [get_hw_devices xc7z020_1]
#  program_hw_devices [get_hw_devices xc7z020_1]
#  refresh_hw_device [lindex [get_hw_devices xc7z020_1] 0]
}
