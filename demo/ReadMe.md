# RISC-V Virtual Platform DEmo Applications

- Change log
- Package structure
- Host platform setup
- Custom environment
- Sample applications
- RISC-V Virtual Platform Configuration

## Change log

- *RISCV-TM* RISC-V *Test Monitor* betta

## Package structure

Package is the tar gzip archive file with the following content inside

```
/ apps
    / hello       - trivial "hello world" example application 
    / spmpX       - RISC-V SPMP demo #X
    / smpuX       - Synopsys SMPU demo #X
    / trapX       - Synopsys RTIA demo #X
/ slib          - Shared Library source code
/ tmon          - Test Monitor source code
    Makefile
    ReadMe.md
    riscv-vp      - RISC-V Virtual Platform executable
/ 
ReadMe.md         - This file
```

## Host platform setup

The RISC-V Virtual Platform requires host computer with fresh Ubuntu 22.04 installation and working connection to the internet. Other host platforms are not yet verified/supported.

Before test applications can be installed, the following preparation steps are rewuired on host PC:

- Mandatory pacakages installation
```
$ sudo apt-get install git autoconf automake autotools-dev curl python3 python3-pip libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev libslirp-dev libncurses-dev libgoogle-perftools-dev libboost-iostreams-dev libboost-program-options-dev libboost-log-dev
``` 
- Create work folder
```
$ mkdir ~/Projects
$ cd ~/Projects
```
- Build the RISC-V GNU toolchain 
```
$ sudo mkdir /opt/riscv-gnu-toolchain
$ sudo chmod 777 /opt/riscv-gnu-toolchain
$ git clone  https://github.com/riscv-collab/riscv-gnu-toolchain.git
$ cd ~/Projects/riscv-gnu-toolchain
$ ./configure --prefix=/opt/riscv-gnu-toolchain --with-arch=rv32i --with-abi=ilp32
$ make
```
- Unpack example source code and simulator executable
```
$ tar xzfv smpu-ifx-drop -C ~/Projects
$ cd ~/Projects/demo
```

Now the host platform is ready to build and execute the sample applications in RISC-V Virtual Platform environment.

## Custom Environment

If custom path/version is used for RISC-V toolchain, then environment variables should be defined to override the default values in demo application's Makefile:
```
$ export SIM_PATH=custom_path_to_riscv_vp_executable
$ export GNU_PATH=custom_path_to_riscv_gnu_toolchain
$ export LIB_PATH=custom_path_to_riscv_gcc_library
``` 

## Sample application

Each example application consists of 
- application source code 
- simple Makefile capable of build, run and debug the application
- ReadMe file 

See the ReadMe files in each application folder for details.

## RISC-V Virtual Platform Configuration

- Memory Map
```
RAM                     0x00000000:0x00100000   
MMIO base address       0x02000000
S-mode SWI address      0x0200C000
HTIF base address       0x02010000
M-mode IMSIC            0x31000000
S-mode IMSIC            0x31001000
```
- Capabilities
```
ISA support:        rv32i
Privilege levels:   M/S/U   (HS/VS/VU support is there but should not be used) 
Memory Isolation:   S-mode MPU
IA architecture:    RTIA
Stsp extension:     Yes
Sswi extension:     Yes
Sstcmp extension:   Yes
```