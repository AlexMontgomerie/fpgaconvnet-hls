#!/bin/bash
TEST_TYPE=all

FPGACONVNET_HLS_PATH=$PWD/../../fpgaconvnet/hls

while getopts ":m:n:cseih" opt; do
    case ${opt} in
        m )
            MODULE=$OPTARG
            ;;
        n )
            TEST_NUM=$OPTARG
            ;;
        c )
            # c simulation
            TEST_TYPE=sim
            ;;
        s )
            # synthesis
            TEST_TYPE=synth
            ;;
        e )
            # co-simulation
            TEST_TYPE=cosim
            ;;
        i )
            # implementation
            TEST_TYPE=impl
            ;;
        h )
            echo "USAGE: run_test.sh [-m (module)] [-n (test number)] [-c,-s,-e,-i]"
            echo "  -c = C simulation"
            echo "  -s = Synthesis"
            echo "  -e = Co-simulation"
            echo "  -i = Implementation"
            exit
            ;;
    esac
done
shift $((OPTIND -1))

function run_test {
    echo "RUNNING TEST ${1}"
    # GENERATE INPUTS
    mkdir -p data/test_${1}
    # python3 gen_data.py -c config/config_${TEST_NUM}.json -o $FPGACONVNET_HLS/test/modules/$MODULE/data/test_${TEST_NUM} -h tb
    python3 gen_data.py -c config/config_${1}.json -o $PWD/data/test_${1} -h tb
    # RUN TEST
    vivado_hls -f ../run_module_hls.tcl "_ -num ${1} -type ${TEST_TYPE} -name ${MODULE} -module_flag -fast"

}

# move to folder
cd $MODULE

if [ $TEST_NUM ]; then

    # echo "RUNNING TEST ${TEST_NUM}"
    # # GENERATE INPUTS
    # mkdir -p data/test_${TEST_NUM}
    # # python3 gen_data.py -c config/config_${TEST_NUM}.json -o $FPGACONVNET_HLS/test/modules/$MODULE/data/test_${TEST_NUM} -h tb
    # python3 gen_data.py -c config/config_${TEST_NUM}.json -o data/test_${TEST_NUM} -h tb
    # # RUN TEST
    # vivado_hls -f $FPGACONVNET_HLS_PATH/scripts/run_hls.tcl "_ -num ${TEST_NUM} -type ${TEST_TYPE} -name ${MODULE} -module_flag -fast"
    run_test $TEST_NUM
# GENERATE REPORTS
python3 ../report.py -m $MODULE -n $TEST_NUM

else

    # NUMBER OF TESTS
    NUM_TEST="$(ls config/ -1U | wc -l)"
    # ITERATE OVER TESTS
    for i in $( seq 0 $(($NUM_TEST-1)) )
    do
        echo "RUNNING TEST ${i}"
        # GENERATE INPUTS
        mkdir -p data/test_${i}
        python3 gen_data.py -c config/config_${i}.json -o $FPGACONVNET_HLS/test/modules/$MODULE/data/test_${i} -h tb
        # RUN TEST
        #vivado_hls -f ../run_test_module.tcl -test ${i} $TEST_TYPE $MODULE
        vivado_hls -f $FPGACONVNET_HLS/scripts/run_hls.tcl "_  -num ${i} -type ${TEST_TYPE} -name ${MODULE} -module_flag "
    done

# GENERATE REPORTS
python3 ../report.py -m $MODULE

fi
