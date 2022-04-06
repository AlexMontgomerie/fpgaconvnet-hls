
class GenerateModule:

    def create_top_includes(self):
        includes = [
                f"#define {self.name.upper()}_{var.upper()} {self.__dict__[var]}" \
                        for var in self.__dict__ ]
        return includes

    def create_top_function(self, io, module):
        # create arguments for top level source
        args = []
        for name in io:
            if len(io[name]) == 0:
                args += [f"stream_t(data_t) &{name}"]
            else:
                dim = "][".join(io[name])
                args += [f"stream_t(data_t) {name}[{dim}]"]
        args = ",\n\t".join(args)

        # interface pragma
        interface_pragma = "\n".join([
            f"\t#pragma HLS INTERFACE axis port={name}" for name in io])

        # create function
        return f"""
void {self.name}_top(
\t{args}
) {{

{interface_pragma}

\t#pragma HLS DATAFLOW

{module}

}}
        """
