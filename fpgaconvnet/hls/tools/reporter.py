import json
import os
import sys

from tabulate import tabulate

class report:

    def __init__(self, report_name):

        # name of report
        self.report_name = report_name

        # initialise tables
        self.pass_results = []
        self.latency_results = []
        self.resource_results = []
        self.clock_results = []

    def append_table_pass(self, test_num, result_path):
        # get result
        with open(result_path,'r') as f:
            result = json.load(f)
        # append to pass results
        self.pass_results.append([
                test_num,
                str(result['pass']['csim']),
                str(result['pass']['synth']),
                str(result['pass']['sim']),
                str(result['pass']['impl'])
            ])

    def append_table_latency(self, test_num, model_path, result_path):
        # get result
        with open(result_path,'r') as f:
            result = json.load(f)
        # get model
        with open(model_path,'r') as f:
            model = json.load(f)
        # append to table_latency
        self.latency_results.append([
                test_num,
                model['latency'],
                result['latency']['synth'],
                result['latency']['sim']
            ])

    def append_table_resources(self, test_num, model_path, result_path):
        # get result
        with open(result_path,'r') as f:
            result = json.load(f)
        # get model
        with open(model_path,'r') as f:
            model = json.load(f)
        # append to table_latency
        self.resource_results.append([
                test_num,
                f"{result['resources']['synth']['LUT']} -> {result['resources']['impl']['LUT']} ({model['resources']['LUT']})",
                f"{result['resources']['synth']['FF']} -> {result['resources']['impl']['FF']} ({model['resources']['FF']})",
                f"{result['resources']['synth']['DSP']} -> {result['resources']['impl']['DSP']} ({model['resources']['DSP']})",
                f"{result['resources']['synth']['BRAM']} -> {result['resources']['impl']['BRAM']} ({model['resources']['BRAM']})",
            ])

    def append_table_clk_period(self, test_num, result_path):
        # get result
        with open(result_path,'r') as f:
            result = json.load(f)
        # append to table_clk_period
        self.clock_results.append([
                test_num,
                result['clk_period']
            ])

    def print_report(self, format="simple"):
        # get tables
        print(self.pass_results)
        pass_table = tabulate(self.pass_results, ["test", "c-sim", "synth", "co-sim", "impl"], tablefmt=format)
        latency_table = tabulate(self.latency_results, ["test", "model", "synth", "co-sim"], tablefmt=format)
        resource_table = tabulate(self.resource_results, ["test", "LUT", "FF", "DSP", "BRAM"], tablefmt=format)
        clk_table = tabulate(self.clock_results, ["test", "period"], tablefmt=format)
        return f"""
# {self.report_name}

{pass_table}

{latency_table}

{resource_table}

{clk_table}
"""

if __name__ == "__main__":
    tmp = report("test")
    print( tmp.print_report() )
