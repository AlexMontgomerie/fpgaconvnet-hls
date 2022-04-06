proc generate_hardware { BOARD WR_ON PORT_WIDTH FREQ DEBUG DEBUG_DEPTH } { 

    # create design
    create_bd_design    "design_1"
    current_bd_design   "design_1"

    # setup
    set parentCell [get_bd_cells /]
    set parentObj [get_bd_cells $parentCell]
    set parentType [get_property TYPE $parentObj]  
    set oldCurInst [current_bd_instance .]
    current_bd_instance $parentObj

    # Create interface ports
    set DDR         [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:ddrx_rtl:1.0                      DDR       ]
    set FIXED_IO    [ create_bd_intf_port -mode Master -vlnv xilinx.com:display_processing_system7:fixedio_rtl:1.0  FIXED_IO  ]

    # instantiate all hardware
    set fpgaconvnet_ip  [ create_bd_cell -type ip -vlnv xilinx.com:hls:fpgaconvnet_ip:1.0       fpgaconvnet_ip  ]
    set hp_in           [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1      hp_in           ]
    set hp_out          [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1      hp_out          ]
    set ctrl            [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1      ctrl            ]
    set ps              [ create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5    ps              ]
    set rst             [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0        rst             ]

    # update IP parameters
    set_property -dict [ list \
        CONFIG.HAS_BURST {1} \
    ] [get_bd_intf_pins fpgaconvnet_ip/m_axi_fpgaconvnet_port_in ]

    set_property -dict [ list \
        CONFIG.HAS_BURST {1} \
    ] [get_bd_intf_pins fpgaconvnet_ip/m_axi_fpgaconvnet_port_out]

    if { $WR_ON } { 
        set_property -dict [ list \
            CONFIG.HAS_BURST {1} \
        ] [get_bd_intf_pins fpgaconvnet_ip/m_axi_fpgaconvnet_port_wr]
    }

    if { $WR_ON } { 
        set_property -dict [ list \
           CONFIG.NUM_MI {1} \
           CONFIG.NUM_SI {2} \
           CONFIG.STRATEGY {2} \
        ] $hp_in
    } else {
        set_property -dict [ list \
           CONFIG.NUM_MI {1} \
           CONFIG.NUM_SI {2} \
           CONFIG.STRATEGY {2} \
        ] $hp_in
    }

    set_property -dict [ list \
       CONFIG.NUM_MI {1} \
       CONFIG.NUM_SI {1} \
       CONFIG.STRATEGY {2} \
    ] $hp_out

    set_property -dict [ list \
       CONFIG.NUM_MI {1} \
       CONFIG.NUM_SI {1} \
       CONFIG.S00_HAS_DATA_FIFO {2} \
       CONFIG.S01_HAS_DATA_FIFO {2} \
       CONFIG.STRATEGY {0} \
    ] $ctrl

    if       { $BOARD == "xilinx.com:zc702:part0:1.4" } {
        set_property -dict [list CONFIG.preset {ZC702}] $ps
    } elseif { $BOARD == "xilinx.com:zc706:part0:1.4" } {
        set_property -dict [list CONFIG.preset {ZC706}] $ps
    } elseif { $BOARD == "em.avnet.com:zed:part0:1.4" } {
        set_property -dict [list CONFIG.preset {ZedBoard}] $ps
    }

    set_property -dict [list \
        CONFIG.PCW_FPGA0_PERIPHERAL_FREQMHZ $FREQ \
        CONFIG.PCW_USE_FABRIC_INTERRUPT {1} \
        CONFIG.PCW_IRQ_F2P_INTR {1} \
    ] $ps

    # TODO: update for number of ports used
    set_property -dict [list \
        CONFIG.PCW_USE_S_AXI_HP0 {1} \
        CONFIG.PCW_S_AXI_HP0_DATA_WIDTH $PORT_WIDTH
    ] $ps

    set_property -dict [list \
        CONFIG.PCW_USE_S_AXI_HP1 {1} \
        CONFIG.PCW_S_AXI_HP1_DATA_WIDTH $PORT_WIDTH
    ] $ps

    # create connections
    connect_bd_intf_net -intf_net fpgaconvnet_ip_port_in    [get_bd_intf_pins fpgaconvnet_ip/m_axi_fpgaconvnet_port_in  ]   [get_bd_intf_pins hp_in/S00_AXI  ]
    connect_bd_intf_net -intf_net fpgaconvnet_ip_port_out   [get_bd_intf_pins fpgaconvnet_ip/m_axi_fpgaconvnet_port_out ]   [get_bd_intf_pins hp_out/S00_AXI ]
    if { $WR_ON } { 
        connect_bd_intf_net -intf_net fpgaconvnet_ip_port_wr    [get_bd_intf_pins fpgaconvnet_ip/m_axi_fpgaconvnet_port_wr  ]   [get_bd_intf_pins hp_in/S01_AXI  ]
    }
    connect_bd_intf_net -intf_net hp_in_connect             [get_bd_intf_pins hp_in/M00_AXI                             ]   [get_bd_intf_pins ps/S_AXI_HP0   ]
    connect_bd_intf_net -intf_net hp_out_connect            [get_bd_intf_pins hp_out/M00_AXI                            ]   [get_bd_intf_pins ps/S_AXI_HP1   ]
    connect_bd_intf_net -intf_net ps_DDR                    [get_bd_intf_ports DDR                                      ]   [get_bd_intf_pins ps/DDR         ]
    connect_bd_intf_net -intf_net ps_FIXED_IO               [get_bd_intf_ports FIXED_IO                                 ]   [get_bd_intf_pins ps/FIXED_IO    ]
    connect_bd_intf_net -intf_net ps_ctrl                   [get_bd_intf_pins ps/M_AXI_GP0                              ]   [get_bd_intf_pins ctrl/S00_AXI   ]
    connect_bd_intf_net -intf_net ctrl_ip                   [get_bd_intf_pins fpgaconvnet_ip/s_axi_ctrl                 ]   [get_bd_intf_pins ctrl/M00_AXI   ]

    connect_bd_net [get_bd_pins fpgaconvnet_ip/interrupt] [get_bd_pins ps/IRQ_F2P]

    if { $WR_ON } { 
        connect_bd_net -net pl_clk \
            [get_bd_pins fpgaconvnet_ip/ap_clk  ] \
            [get_bd_pins hp_in/ACLK             ] \
            [get_bd_pins hp_out/ACLK            ] \
            [get_bd_pins hp_in/M00_ACLK         ] \
            [get_bd_pins hp_out/M00_ACLK        ] \
            [get_bd_pins hp_in/S00_ACLK         ] \
            [get_bd_pins hp_in/S01_ACLK         ] \
            [get_bd_pins hp_out/S00_ACLK        ] \
            [get_bd_pins ps/FCLK_CLK0           ] \
            [get_bd_pins ps/M_AXI_GP0_ACLK      ] \
            [get_bd_pins ps/S_AXI_HP0_ACLK      ] \
            [get_bd_pins ps/S_AXI_HP1_ACLK      ] \
            [get_bd_pins ctrl/ACLK              ] \
            [get_bd_pins ctrl/M00_ACLK          ] \
            [get_bd_pins ctrl/S00_ACLK          ] \
            [get_bd_pins rst/slowest_sync_clk   ]
    } else {
        connect_bd_net -net pl_clk \
            [get_bd_pins fpgaconvnet_ip/ap_clk  ] \
            [get_bd_pins hp_in/ACLK             ] \
            [get_bd_pins hp_out/ACLK            ] \
            [get_bd_pins hp_in/M00_ACLK         ] \
            [get_bd_pins hp_out/M00_ACLK        ] \
            [get_bd_pins hp_in/S00_ACLK         ] \
            [get_bd_pins hp_out/S00_ACLK        ] \
            [get_bd_pins ps/FCLK_CLK0           ] \
            [get_bd_pins ps/M_AXI_GP0_ACLK      ] \
            [get_bd_pins ps/S_AXI_HP0_ACLK      ] \
            [get_bd_pins ps/S_AXI_HP1_ACLK      ] \
            [get_bd_pins ctrl/ACLK              ] \
            [get_bd_pins ctrl/M00_ACLK          ] \
            [get_bd_pins ctrl/S00_ACLK          ] \
            [get_bd_pins rst/slowest_sync_clk   ]
    }

    connect_bd_net -net ps_reset \
        [get_bd_pins ps/FCLK_RESET0_N] \
        [get_bd_pins rst/ext_reset_in]

    connect_bd_net -net interconnect_reset \
        [get_bd_pins hp_in/ARESETN  ] \
        [get_bd_pins hp_out/ARESETN ] \
        [get_bd_pins ctrl/ARESETN   ] \
        [get_bd_pins rst/interconnect_aresetn]

    if { $WR_ON } { 
        connect_bd_net -net peripheral_reset \
            [get_bd_pins fpgaconvnet_ip/ap_rst_n] \
            [get_bd_pins hp_in/M00_ARESETN      ] \
            [get_bd_pins hp_out/M00_ARESETN     ] \
            [get_bd_pins hp_in/S00_ARESETN      ] \
            [get_bd_pins hp_in/S01_ARESETN      ] \
            [get_bd_pins hp_out/S00_ARESETN     ] \
            [get_bd_pins ctrl/M00_ARESETN       ] \
            [get_bd_pins ctrl/S00_ARESETN       ] \
            [get_bd_pins rst/peripheral_aresetn ]
    } else {
        connect_bd_net -net peripheral_reset \
            [get_bd_pins fpgaconvnet_ip/ap_rst_n] \
            [get_bd_pins hp_in/M00_ARESETN      ] \
            [get_bd_pins hp_out/M00_ARESETN     ] \
            [get_bd_pins hp_in/S00_ARESETN      ] \
            [get_bd_pins hp_out/S00_ARESETN     ] \
            [get_bd_pins ctrl/M00_ARESETN       ] \
            [get_bd_pins ctrl/S00_ARESETN       ] \
            [get_bd_pins rst/peripheral_aresetn ]
    }

    # update base addresses (auto assign)
    assign_bd_address

    if { $DEBUG } {
        ## add debug probes
        #set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets {fpgaconvnet_ip_out}]
        #set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets {fpgaconvnet_ip_in}]
        #
        #apply_bd_automation -rule xilinx.com:bd_rule:debug -dict [list \
        #    [get_bd_intf_nets fpgaconvnet_ip_in] {AXIS_SIGNALS "Data and Trigger" CLK_SRC "/processing_system7_0/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
        #    [get_bd_intf_nets fpgaconvnet_ip_out] {AXIS_SIGNALS "Data and Trigger" CLK_SRC "/processing_system7_0/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
        #]
        #set_property -dict [list CONFIG.C_BRAM_CNT {5} CONFIG.C_DATA_DEPTH $DEBUG_DEPTH ] [get_bd_cells system_ila_0]
    }
    # save design
    current_bd_instance $oldCurInst
    save_bd_design
    
    # regenerate layout
    regenerate_bd_layout

    # validate design
    validate_bd_design

}
