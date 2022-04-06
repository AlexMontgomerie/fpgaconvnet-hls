import re
import untangle
import json
import os
import sys
import yaml

def check_csim_log(path):
    if not os.path.isfile(path):
        return False
    error_line = re.compile("INFO: \[SIM 1\] CSim done with ([0-9]+) errors.")
    with open(path,'r') as f:
        for line in f:
            num_err = error_line.match(line)
            if num_err:
                num_err = int(num_err.group(1))
                break
    if num_err == 0:
        return True
    return False

def get_model_latency(path):
    with open(path,'r') as f:
        data = yaml.load(f,Loader=yaml.Loader)    
    return data['latency'] 

def get_model_resources(res,path):
    with open(path,'r') as f:
        data = yaml.load(f,Loader=yaml.Loader)    
    res['resource_model']['LUT']  = data['LUT'] 
    res['resource_model']['FF']   = data['FF'] 
    res['resource_model']['DSP']  = data['DSP']
    res['resource_model']['BRAM'] = data['BRAM']
    return

def get_synth_latency(path):
    obj = untangle.parse(path)
    return int(obj.profile.PerformanceEstimates.SummaryOfOverallLatency.Average_caseLatency.cdata)

def get_sim_latency(path):
    latency_line = re.compile("\|   Verilog\|      Pass\|(?: +)(?:[0-9]+)\|(?: +)([0-9]+)\|")
    with open(path,'r') as f:
        for line in f:
            latency = latency_line.match(line)
            if latency:
                return int(latency.group(1))
    return 0

def get_resources(res,path):
    obj = untangle.parse(path)
    res['resource']['SLICE'] = int(obj.profile.AreaReport.Resources.SLICE.cdata)    
    res['resource']['LUT']   = int(obj.profile.AreaReport.Resources.LUT.cdata)    
    res['resource']['FF']    = int(obj.profile.AreaReport.Resources.FF.cdata)    
    res['resource']['DSP']   = int(obj.profile.AreaReport.Resources.DSP.cdata)    
    res['resource']['BRAM']  = int(obj.profile.AreaReport.Resources.BRAM.cdata)    
    res['resource']['SRL']   = int(obj.profile.AreaReport.Resources.SRL.cdata)    

def get_timing(res,path):
    obj = untangle.parse(path)
    res['timing'] = float(obj.profile.TimingReport.AchievedClockPeriod.cdata)    
 
def get_description(test_num):
    with open("config/config_{}.json".format(test_num), 'r') as f:
        return json.load(f)['description']

def get_logs(name,extension=""):
    # test results
    results = []
    # iterate over tests
    for filename in os.listdir("{name}_hls_prj".format(name=name)):
        # get test number    
        test_num = re.match('solution([0-9]+)',filename)
        if test_num == None:
            continue 
        test_num = test_num.group(1)
        # initialise results
        res = {
            'test_num'      : test_num,
            'description'   : get_description(test_num),
            'baseline' : {
                'csim'  : False,
                'synth' : False,
                'sim'   : False,
                'impl'  : False
            },
            'latency' : {
                'model' : 0,
                'synth' : 0,
                'sim'   : 0
            },
            'resource' : {
                'SLICE' : 0,
                'LUT'   : 0,
                'FF'    : 0,
                'DSP'   : 0,
                'BRAM'  : 0,
                'SRL'   : 0
            },
            'resource_model' : {
                'LUT'   : 0,
                'FF'    : 0,
                'DSP'   : 0,
                'BRAM'  : 0,
            },

            'timing': 0
        }
        # Baseline
        ## check csim passed
        res['baseline']['csim'] = check_csim_log('{name}_hls_prj/solution{test_num}/csim/report/{name}{extension}_top_csim.log'.format(
            name=name,test_num=test_num,extension=extension))
        ## check synth passed
        res['baseline']['synth'] = os.path.isfile('{name}_hls_prj/solution{test_num}/syn/report/csynth.xml'.format(
            name=name,test_num=test_num))
        ## check sim passed
        res['baseline']['sim'] = os.path.isfile('{name}_hls_prj/solution{test_num}/sim/report/{name}{extension}_top_cosim.rpt'.format(
            name=name,test_num=test_num,extension=extension))
        print(res['baseline']['sim']
        ## check impl passed
        res['baseline']['impl'] = os.path.isfile('{name}_hls_prj/solution{test_num}/impl/report/verilog/{name}{extension}_top_export.xml'.format(
            name=name,test_num=test_num,extension=extension))
        # Latency
        ## get model latency
        res['latency']['model'] = get_model_latency('data/test_{test_num}/data.yaml'.format(test_num=test_num))
        ## get synth latency
        if res['baseline']['synth']:
            res['latency']['synth'] = get_synth_latency('{name}_hls_prj/solution{test_num}/syn/report/csynth.xml'.format(
                name=name,test_num=test_num))
        ## get sim latency
        if res['baseline']['sim']:
            res['latency']['sim'] = get_sim_latency('{name}_hls_prj/solution{test_num}/sim/report/{name}{extension}_top_cosim.rpt'.format(
                name=name,test_num=test_num,extension=extension))
        # Resource & Timing
        # TODO
        if res['baseline']['impl']:
            get_model_resources(res,'data/test_{test_num}/data.yaml'.format(test_num=test_num))
            get_resources(res,'{name}_hls_prj/solution{test_num}/impl/report/verilog/{name}{extension}_top_export.xml'.format(
                name=name,test_num=test_num,extension=extension))
            get_timing(res,'{name}_hls_prj/solution{test_num}/impl/report/verilog/{name}{extension}_top_export.xml'.format(
                name=name,test_num=test_num,extension=extension))
        # append results
        results.append(res)
    # save results
    for res in results:
        with open('rpt/test_{}.json'.format(res['test_num']),'w') as f:
            json.dump(res,f,indent=2)

if __name__ == "__main__":
    if len(sys.argv) == 3:
        get_logs(sys.argv[1],extension=sys.argv[2])
    else:
        get_logs(sys.argv[1])
        
