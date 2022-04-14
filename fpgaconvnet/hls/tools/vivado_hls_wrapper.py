import subprocess

class VivadoHLSWrapper:

    def __init__(self, name, project_path):
        self.session = subprocess.call(["vivado_hls", "-i"], stdin=subprocess.PIPE,
                stdout=subprocess.PIPE)
        self.wait_for_command()

    def wait_for_command(self):
        out = self.session.stdout.readline().decode()
        while out != "vivado_hls>":
            out = self.session.stdout.readline().decode()
            print(out)
            print(self.session.stdout.read().decode())

    def open_project(self, project_path, reset=True):
        if reset:
            self.session.communicate(input=f"open_project -reset {project_path}\n".encode())
        else:
            self.session.communicate(input=f"open_project {project_path}\n".encode())

    def set_top(self, top):
        self.session.communicate(input=f"set_top {top}\n".encode())

    def open_solution(self, solution, reset=True):
        if reset:
            self.session.communicate(input=f"open_solution -reset '{solution}'\n".encode())
        else:
            self.session.communicate(input=f"open_solution '{solution}'\n".encode())

    def run_csynth(self):
        self.session.communicate(input='csynth_design\n'.encode())

    def exit(self):
        self.session.communicate(input='exit\n'.encode())

    def __del__(self):
        self.exit()
        self.session.terminate()

if __name__ == "__main__":
    tmp = VivadoHLSWrapper()
    tmp.open_project("fork_hls_prj", reset=False)
    tmp.set_top("fork_top")
    tmp.open_solution("solution0", reset=False)
    tmp.run_csynth()
