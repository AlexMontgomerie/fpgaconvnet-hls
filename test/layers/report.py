import re
import os
import json
import argparse
from fpgaconvnet.hls.tools.hls_logger import hls_log
from fpgaconvnet.hls.tools.reporter import report

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Layer Report Generator")
    parser.add_argument('-l','--layer', required=True, help='Name of layer')
    parser.add_argument('-n','--number', default=-1, help='test config number')
    args = parser.parse_args()

    # make the path
    output_path = f"rpt/"
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    # create a report instance
    layer_report = report(f"{args.layer} report")

    if args.number == -1:
        test_pool = [config_file for config_file in os.listdir("config")]
    else:
        #do a single report
        test_pool = [args.number]

    for i in range(len(test_pool)):
        # get test number
        if args.number == -1:
            test_num = int(re.search(r"\d+",test_pool[i]).group(0))
        else:
            test_num = args.number

        # get hls logs
        layer_test_log = hls_log(f"{args.layer}_layer_top", f"{args.layer}_hls_prj/solution{test_num}")
        # generate results of test
        results = {
            "pass" : layer_test_log.check_pass(),
            "latency" : {
                "synth" : "N/A", #layer_test_log.get_synth_latency(),
                "sim"   : "N/A", #layer_test_log.get_sim_latency(),
            },
            "resources"  : {
                "impl"  : {
                    "BRAM"  : "N/A",
                    "LUT"   : "N/A",
                    "FF"    : "N/A",
                    "DSP"   : "N/A",
                },
                "synth"  : {
                    "BRAM"  : "N/A",
                    "LUT"   : "N/A",
                    "FF"    : "N/A",
                    "DSP"   : "N/A",
                },
            },
            "clk_period" : "N/A", #layer_test_log.get_clk_period()
        }
        # update latency
        try:
            results["latency"]["synth"] = layer_test_log.get_synth_latency()
            results["resources"]["synth"] = layer_test_log.get_synth_resources()
        except AssertionError:
            pass
        try:
            results["latency"]["sim"] = layer_test_log.get_sim_latency()
        except AssertionError:
            pass
        try:
            results["resources"]["impl"] = layer_test_log.get_impl_resources()
            results["clk_period"] = layer_test_log.get_clk_period()
        except AssertionError:
            pass
        # save results
        with open(f"rpt/result_{test_num}.json","w") as f:
            json.dump(results,f)
        # append to layer report
        result_path = f"rpt/result_{test_num}.json"
        model_path  = f"data/test_{test_num}/model.json"
        if os.path.exists(result_path) and os.path.exists(model_path):
            layer_report.append_table_pass(test_num, result_path)
            layer_report.append_table_latency(test_num, model_path, result_path)
            layer_report.append_table_resources(test_num, model_path, result_path)
            layer_report.append_table_clk_period(test_num, result_path)

    # save report
    print(layer_report.print_report())
    with open("REPORT.md","w") as f:
        f.write(layer_report.print_report(format="github"))
