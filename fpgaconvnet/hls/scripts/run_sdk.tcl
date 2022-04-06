## how to run 
#  xsct run_sdk.tcl [hw_def_path] 
# NOTE: if workspace in use error, remove /tmp/workspace/.metadata/.lock 

## commands
# -create   = create all project files
# -clean    = deletes all projects in workspace
# -update   = update all project files

# Set SDK workspace
setws -switch $::env(FPGACONVNET_HLS)/host

# get the device
set device_name zc706
#set device_name zedboard

# Initialise project
if {[lsearch -exact $argv -create] >=0} {
    puts " Creating project ..."
   # Create a HW project
    createhw -name fpgaconvnet_hw -hwspec $::env(FPGACONVNET_HLS)/host/devices/${device_name}/fpgaconvnet.hdf
    # Create a BSP project
    createbsp -name fpgaconvnet_bsp -hwproject fpgaconvnet_hw -proc ps7_cortexa9_0 
    setlib    -bsp fpgaconvnet_bsp -lib xilffs
    updatemss -mss $::env(FPGACONVNET_HLS)/host/fpgaconvnet_bsp/system.mss
    regenbsp  -bsp fpgaconvnet_bsp
    # Create application project
    createapp -name fpgaconvnet_prj -hwproject fpgaconvnet_hw -bsp fpgaconvnet_bsp -proc ps7_cortexa9_0 -lang C++ -app {Empty Application}
    importsources -name fpgaconvnet_prj -path $::env(FPGACONVNET_HLS)/host/.srcs -linker-script
}

# Update project
if {[lsearch -exact $argv -update] >=0} {
    puts " Updating project ..."
    # Update a HW project
    updatehw -hw fpgaconvnet_hw -newhwspec $::env(FPGACONVNET_HLS)/host/devices/${device_name}/fpgaconvnet.hdf
    # Update a BSP project
    setlib    -bsp fpgaconvnet_bsp -lib xilffs
    updatemss -mss $::env(FPGACONVNET_HLS)/host/fpgaconvnet_bsp/system.mss
    regenbsp  -bsp fpgaconvnet_bsp
}

# build project
projects -build

# Connect to local hw_server
connect

# Select a target
targets -set -nocase -filter {name =~ "ARM*#0"}

# System Reset
rst 
rst -system 

# Program bitstream
fpga -file $::env(FPGACONVNET_HLS)/host/devices/${device_name}/fpgaconvnet.bit

# PS7 initialization
namespace eval xsdb {
    loadhw $::env(FPGACONVNET_HLS)/host/fpgaconvnet_hw/system.hdf
    source $::env(FPGACONVNET_HLS)/host/fpgaconvnet_hw/ps7_init.tcl
    ps7_init
    ps7_post_config
}

# Insert a breakpoint @ main
#bpadd -addr &main

# Download the elf
dow $::env(FPGACONVNET_HLS)/host/fpgaconvnet_prj/Debug/fpgaconvnet_prj.elf

# set breakpoint at exit
bpadd -addr &exit

# Continue execution until the target is suspended
con -block 

# while {1} {}

#con -block
#con -block -timeout 500 

# Print the target registers
# puts [rrd]

# Resume the target
# con
