#!/bin/bash

# Initialise variables
NETWORK=""
MODEL_PATH=""
WEIGHTS_PATH=""
PARTITION_INFO_PATH=""
ZYNQ_PART=xc7z020clg484-1
ZYNQ_BOARD=xilinx.com:zc702:part0:1.4
TEST_TYPE=impl

# get network arguments
while getopts ":n:m:w:d:p:b:f:cseigh" opt; do
    case ${opt} in
        n ) NETWORK=$OPTARG;;
        m ) MODEL_PATH=$OPTARG;;
        #w ) WEIGHTS_PATH=$OPTARG;;
        d ) IMAGE_PATH=$OPTARG;;
        p ) PARTITION_INFO_PATH=$OPTARG;;
        b ) ZYNQ_BOARD=$OPTARG;;
        f ) ZYNQ_PART=$OPTARG;;
        c ) TEST_TYPE=sim;;
        s ) TEST_TYPE=synth;;
        e ) TEST_TYPE=cosim;;
        i ) TEST_TYPE=impl;;
        g ) TEST_TYPE=gen_hw;;
        h )
            echo "USAGE: run_test.sh [-n (network name)] [-m (prototxt path)] [-w (caffemodel path)] [-d (image path)] "
            echo "                   [-p (partition info path)] [-b (board)] [-f (fpga)] [-c,-s,-e,-i,-g]"
            echo "  -c = C simulation"
            echo "  -s = Synthesis"
            echo "  -e = Co-simulation"
            echo "  -i = Implementation"
            echo "  -g = Design Generation"
            exit
            ;;
    esac
done
shift $((OPTIND -1))

# move into network folder
#cd $NETWORK

# create outputs folder
mkdir -p outputs

# get the number of partitions
NUM_PARTITIONS=$( jq '.partition | length' $PARTITION_INFO_PATH )

echo "Expecting ${NUM_PARTITIONS} partitions" #Debug

# iterate over partitions
for i in $( seq 1 ${NUM_PARTITIONS}); do

    # get current partition index
    PARTITION_INDEX=$(( $i - 1))

    # create folders
    mkdir -p partition_${PARTITION_INDEX}
    mkdir -p partition_${PARTITION_INDEX}/tb
    mkdir -p partition_${PARTITION_INDEX}/src
    mkdir -p partition_${PARTITION_INDEX}/include
    mkdir -p partition_${PARTITION_INDEX}/data

    # get weights reloading factor
    WEIGHTS_RELOADING_FACTOR=$( jq .partition[$PARTITION_INDEX].weights_reloading_factor $PARTITION_INFO_PATH )
    WEIGHTS_RELOADING_LAYER=$( jq .partition[$PARTITION_INDEX].weights_reloading_layer $PARTITION_INFO_PATH )
    WEIGHTS_RELOADING_FLAG=1
    if [ "$WEIGHTS_RELOADING_LAYER" == "None" ]; then
        WEIGHTS_RELOADING_FLAG=0
    fi

    # port information
    PORT_WIDTH=64 # TODO: get from partition information file
    #PORTS_IN=$( jq .[$PARTITION_INDEX].partition_info.ports_in $PARTITION_INFO_PATH )
    #PORTS_OUT=$( jq .[$PARTITION_INDEX].partition_info.ports_out $PARTITION_INFO_PATH )

    # partition frequency
    FREQ=125 # TODO: get from partition information file

    # create hardware
    python $FPGACONVNET_HLS/scripts/generate_hardware.py -n $NETWORK -p $PARTITION_INFO_PATH -i $PARTITION_INDEX

    # format weights
    python $FPGACONVNET_HLS/scripts/format_weights.py -p $PARTITION_INFO_PATH -i $PARTITION_INDEX -m $MODEL_PATH

    # format featuremaps
    # python $FPGACONVNET_HLS/scripts/format_featuremaps.py -m $MODEL_PATH -p $PARTITION_INFO_PATH -d $IMAGE_PATH -i $PARTITION_INDEX

    # run the network
    cd partition_${PARTITION_INDEX}
        if [ "$TEST_TYPE" = "gen_hw" ]; then
            # create fpgaconvnet partition ip
            vivado_hls -f $FPGACONVNET_HLS/scripts/run_hls.tcl "_  -type impl -name ${NETWORK} -fpga ${ZYNQ_PART} -network_flag -reset -fast"
            # create bitstream for given platform
            vivado -mode batch -notrace -source $FPGACONVNET_HLS/scripts/gen_hw.tcl \
                -tclargs $NETWORK $ZYNQ_PART $ZYNQ_BOARD $FREQ $PORT_WIDTH $WEIGHTS_RELOADING_FLAG
            # copy bitstreams for partition, as well as hardware platform definition
            cp ${NETWORK}_hw_prj/project_1/design_1_wrapper.bin                         ../outputs/p${PARTITION_INDEX}.bin
            cp ${NETWORK}_hw_prj/project_1/project_1.runs/impl_1/design_1_wrapper.bit   ../outputs/p${PARTITION_INDEX}.bit
            cp ${NETWORK}_hw_prj/project_1/project_1.runs/impl_1/design_1_wrapper.hwdef ../outputs/p${PARTITION_INDEX}.hwdef
        else
            # run hls only
            vivado_hls -f $FPGACONVNET_HLS/scripts/run_hls.tcl "_  -type ${TEST_TYPE} -name ${NETWORK} -fpga ${ZYNQ_PART} -network_flag -reset"
        fi
    cd ..

done
cd ..

