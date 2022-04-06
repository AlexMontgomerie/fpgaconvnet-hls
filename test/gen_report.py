import json
import os
import sys

def get_baseline_results(res):
    out = ""
    for result in res:
        out+="| {test_num} | {csim} | {synth} | {sim} | {impl} | \n".format(
            test_num=result['test_num'],
            csim =result['baseline']['csim'],
            synth=result['baseline']['synth'],
            sim  =result['baseline']['sim'],
            impl =result['baseline']['impl']
        )
    return out

def get_latency_results(res):
    out = ""
    for result in res:
        out+="| {test_num} | {model} | {synth} | {sim} | \n".format(
            test_num=result['test_num'],
            model=result['latency']['model'],
            synth=result['latency']['synth'],
            sim  =result['latency']['sim']
        )
    return out

def get_resource_results(res):
    out = ""
    for result in res:
        out+="| {test_num} | {LUT} | {FF} | {DSP} | {BRAM} || {LUT_model} | {FF_model} | {DSP_model} | {BRAM_model} | \n".format(
            test_num=result['test_num'],
            LUT =result['resource']['LUT'],
            FF  =result['resource']['FF'],
            DSP =result['resource']['DSP'],
            BRAM=result['resource']['BRAM'],
            LUT_model =int(result['resource_model']['LUT'] ),
            FF_model  =int(result['resource_model']['FF']  ),
            DSP_model =int(result['resource_model']['DSP'] ),
            BRAM_model=int(result['resource_model']['BRAM']),


        )
    return out

def get_timing_results(res):
    out = ""
    for result in res:
        out+="| {test_num} | {clk} | \n".format(
            test_num=result['test_num'],
            clk=result['timing']
        )
    return out

name=sys.argv[1]

# load results
res = []
for filename in os.listdir("rpt"):
    with open("rpt/"+filename,'r') as f:
        res.append(json.load(f))

report_template = """
# {name} Report

## Baseline Tests

| TEST | CSIM | SYNTH | SIM | IMPL |
|:----:|:----:|:-----:|:---:|:----:|
{baseline_results}


## Latency Results

| TEST | MODEL | SYNTH | SIM |
|:----:|:-----:|:-----:|:---:|
{latency_results}


## Resource Results

| TEST | LUT | FF | DSP | BRAM || LUT (model) | FF (model) | DSP (model) | BRAM (model) |
|:----:|:---:|:--:|:---:|:----:||:---:|:--:|:---:|:----:|
{resource_results}

## Timing Results

| TEST | CLK |
|:----:|:---:|
{timing_results}

## Warnings

{warnings}

""".format(
    name=name.upper(),
    baseline_results=get_baseline_results(res),
    latency_results =get_latency_results(res),
    resource_results=get_resource_results(res),
    timing_results  =get_timing_results(res),
    warnings        =""
)

# save file
with open('REPORT.md','w') as f:
    f.write(report_template)
