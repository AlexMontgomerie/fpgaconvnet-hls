# fpgaConvNet HLS

This is CNN-to-FPGA mapping framework designed to find the optimal implementation of a CNN architecture on an FPGA for power, latency and throughput driven designs.

## Setup

_This project has only been verified on Ubuntu 18.04_

The following programmes are needed:

1. Vivado 2019.1
2. Anaconda
3. yaml-cpp (version 0.5.3)

Also, you will need to add the environmental variable `FPGACONVNET_OPTIMISER` that points to the install directory of [fpgaconvnet-hls](https://github.com/AlexMontgomerie/fpgaconvnet-optimiser).

> You will need to use the same python environment as fpgaconvnet-hls also

### Vivado Setup

To install Vivado 2019.1, first [download](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/2019-1.html) from the Xilinx website.

Once installed, you will need to add a license server to your .bashrc file. You will also need to add Vivado to your path. To do so, add the following to your .basrc:

```
export PATH=/tools/Xilinx/Vivado/2019.1/bin:$PATH
export PATH=/tools/Xilinx/SDK/2019.1/bin:$PATH
```


You will need to setup JTAG drivers to program a device. To do so, execute the following script:

```
/tools/Xilinx/Vivado/2019.1/data/xicom/cable_drivers/lin64/install_script/install_drivers/install_drivers
```

For more information, visit [here](https://www.xilinx.com/support/answers/59128.html).

Finally, there is a known [bug](http://svn.clifford.at/handicraft/2017/vivadobugs/vivadobug04.txt) to do with C++ libraries. A workaround for this is adding the `mpfr.h` and `gmp.h` headers manually. For this project, you need to create a header file `include/system.hpp` which includes the following:

```C
#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include "(path to Vivado 2019.1)/include/gmp.h"
#include "(path to Vivado 2019.1)/include/mpfr.h"

#endif
```

