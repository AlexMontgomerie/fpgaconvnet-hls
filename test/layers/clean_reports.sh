#!/bin/bash

function clean_prj {
    if [ $1 ]; then
        rm ${1}/rpt/*
        rm -rf ${1}/data/*
        rm -rf ${1}/${1}_hls_prj/*
    fi
}

clean_prj concat 
clean_prj relu
clean_prj convolution
clean_prj pooling 

