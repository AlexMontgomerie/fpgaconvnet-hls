#!/bin/bash

function clean_prj {
    if [ $1 ]; then
        rm ${1}/rpt/*
        rm -rf ${1}/data/*
        rm -rf ${1}/${1}_hls_prj/*
        rm -rf ${1}/${1}_test_prj/*
    fi
}

clean_prj accum
clean_prj conv 
clean_prj fork
clean_prj glue 
clean_prj split 
clean_prj relu 
clean_prj sliding_window 

