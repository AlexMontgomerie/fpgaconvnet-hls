#!/bin/bash
TEST_TYPE=all

FPGACONVNET_HLS_PATH=$PWD/../../fpgaconvnet/hls

while getopts ":l:n:cseih" opt; do
    case ${opt} in
        l )
            LAYER=$OPTARG
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
        a )
            # implementation
            TEST_TYPE=all
            ;;
        h )
            echo "USAGE: run_test.sh [-l (layer)] [-n (test number)] [-c,-s,-e,-i,-a]"
            echo "  -c = C simulation"
            echo "  -s = Synthesis"
            echo "  -e = Co-simulation"
            echo "  -i = Implementation"
            echo "  -a = All"
            exit
            ;;
    esac
done
shift $((OPTIND -1))

function run_test {

    echo "RUNNING TEST ${1}"
    # GENERATE INPUTS
    mkdir -p include
    mkdir -p data/test_${1}
    python gen_layer.py -c config/config_${1}.json -o $PWD/data/test_${1} -s src/ -h include/ -t tb/
    # RUN TEST
    vivado_hls -f ../run_layer_hls.tcl "_  -num ${1} -type ${TEST_TYPE} -name ${LAYER}"

}

# Move to folder
cd $LAYER

if [ $TEST_NUM ]; then

    # run test
    run_test $TEST_NUM

    # generate reports
    python ../report.py -l $LAYER -n $TEST_NUM

else

    # NUMBER OF TESTS
    NUM_TEST="$(ls config/ -1U | wc -l)"
    # ITERATE OVER TESTS
    for i in $( seq 0 $(($NUM_TEST-1)) ); do

        # run test
        run_test $i

    done

    # GENERATE REPORTS
    python ../report.py -l $LAYER

fi
