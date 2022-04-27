import re
import uuid
import untangle
import json
import os
import sys
import yaml

class hls_log():
    """
    class to parse and format outputs from HLS projects.
    """

    def __init__(self, design_name, solution_path, lang="verilog"):

        self.lang = lang
        self.design_name = design_name
        self.solution_path = solution_path

        # relevant log file paths
        self.csim_log = os.path.join(self.solution_path,"csim/report",
                f"{self.design_name}_csim.log")
        self.synth_xml = os.path.join(self.solution_path,"syn/report/csynth.xml")
        self.cosim_rpt = os.path.join(self.solution_path,"sim/report",
                f"{self.design_name}_cosim.rpt")
        self.impl_xml = os.path.join(self.solution_path,"impl/report",self.lang,
                f"{self.design_name}_export.xml")

    def check_pass_csim(self) -> bool:
        """
        checks whether c-simulation has passed.

        Returns:
        --------
        bool
        """
        assert os.path.exists(self.csim_log)
        error_line = re.compile("INFO: \[SIM 1\] CSim done with ([0-9]+) errors.")
        with open(self.csim_log,"r") as f:
            errors = error_line.findall(f.read(),re.MULTILINE)
            assert len(errors) > 0
            assert int(errors[0]) == 0

    def check_pass_synth(self) -> bool:
        """
        checks whether HLS synthesis has passed.

        Returns:
        --------
        bool
        """
        assert os.path.exists(self.synth_xml)

    def check_pass_sim(self) -> bool:
        """
        checks whether co-simulation has passed.

        Returns:
        --------
        bool
        """
        assert os.path.exists(self.cosim_rpt)

    def check_pass_impl(self) -> bool:
        """
        checks whether the design has been implemented.

        Returns:
        --------
        bool
        """
        assert os.path.exists(self.impl_xml)

    def check_pass(self) -> dict:
        """
        check whether all parts of the HLS project have
        passed.

        Returns:
        --------
        dict
        """

        test_pass = {
            'csim'  : True,
            'synth' : True,
            'sim'   : True,
            'impl'  : True
        }
        # csim
        try:
            self.check_pass_csim()
        except AssertionError:
            test_pass['csim'] = False
        # synth
        try:
            self.check_pass_synth()
        except AssertionError:
            test_pass['synth'] = False
        # sim
        try:
            self.check_pass_sim()
        except AssertionError:
            test_pass['sim'] = False
        # impl
        try:
            self.check_pass_impl()
        except AssertionError:
            test_pass['impl'] = False
        # return all tests
        return test_pass

    def get_impl_resources(self) -> dict:
        """
        get the implementation resources used. This includes
        SLICE, LUT, FF, DSP, BRAM and SRL resources.

        Returns:
        --------
        dict
        """
        assert os.path.exists(self.impl_xml)
        obj = untangle.parse(self.impl_xml)
        return {
            'SLICE' : int(obj.profile.AreaReport.Resources.SLICE.cdata),
            'LUT'   : int(obj.profile.AreaReport.Resources.LUT.cdata),
            'FF'    : int(obj.profile.AreaReport.Resources.FF.cdata),
            'DSP'   : int(obj.profile.AreaReport.Resources.DSP.cdata),
            'BRAM'  : int(obj.profile.AreaReport.Resources.BRAM.cdata),
            'SRL'   : int(obj.profile.AreaReport.Resources.SRL.cdata)
        }

    def get_synth_resources(self) -> dict:
        """
        get the predicted resources used for synthesis. This
        includes LUT, FF, DSP and BRAM resources.

        Returns:
        --------
        dict
        """
        assert os.path.exists(self.synth_xml)
        obj = untangle.parse(self.synth_xml)
        return {
            'LUT'   : int(obj.profile.AreaEstimates.Resources.LUT.cdata),
            'FF'    : int(obj.profile.AreaEstimates.Resources.FF.cdata),
            'DSP'   : int(obj.profile.AreaEstimates.Resources.DSP48E.cdata),
            'BRAM'  : int(obj.profile.AreaEstimates.Resources.BRAM_18K.cdata)
        }

    def get_synth_latency(self) -> int:
        """
        get the predicted latency from HLS synthesis

        Returns:
        --------
        int
        """
        assert os.path.exists(self.synth_xml)
        latency = untangle.parse(self.synth_xml).profile.PerformanceEstimates.SummaryOfOverallLatency.Average_caseLatency.cdata
        if latency == "undef":
            return "N/A"
        else:
            return int(latency)

    def get_sim_latency(self) -> int:
        """
        get the measured latency from co-simulation.

        Returns:
        --------
        int
        """
        assert os.path.exists(self.cosim_rpt)
        latency_line = re.compile("\|   Verilog\|      Pass\|(?: +)(?:[0-9]+)\|(?: +)([0-9]+)\|")
        with open(self.cosim_rpt,"r") as f:
            latencys = latency_line.findall(f.read(), re.MULTILINE)
            if latencys:
                return int(latencys[0])
   # TODO: find which one is latency

    def get_clk_period(self) -> float:
        """
        get the predicted clock period from HLS synthesis

        Returns:
        --------
        float
        """
        assert os.path.exists(self.impl_xml)
        return float(untangle.parse(self.impl_xml).profile.TimingReport.AchievedClockPeriod.cdata)
