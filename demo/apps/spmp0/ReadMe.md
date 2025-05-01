= RISC-V Virtual Platform - RISC-V SPMP demo application #0, static physical memory map

- Folder content

```
 linker.ld           - linker script
 main.c              - test application source file
 Makefile            - build script
 ReadMe.md           - this file
 ``

- Build instruction

In order to (re)build the application run the following commands:

```
$ cd ~/Projects/demo/apps/spmp0
$ make clean all
```
To remove build artifacts run the following command:
```
$ cd ~/Projects/demo/apps/spmp0
$ make clean 
``` 
- Executing example application

The build script provided is capable of executing the elf-file in RISC-V Virtual Platform environment:
```
$ cd ~/Projects/demo/apps/spmp0
$ make run
```
In this mode RISC-V VP starts as standalone application and executes elf-file providing the simulation log and statistic as well as redirecting the program output to the console. In case of successful running you should see output as follows:
```
  TBD
``` 

- Debugging example application

The build script provided is capable of running the RISC-V Virtual Platform as a client for GNU GDB:
```
$ cd ~/Projects/demo/apps/spmp0
$ make dbg
```
The command above runs the simulation in background, loads executable file, and starts GDB with TUI interface and open remote debugging session connected to riscv-vp.
