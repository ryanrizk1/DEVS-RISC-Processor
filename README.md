# DEVS RISC Processor
|  Linux | Windows|
|:--|:--|
|:heavy_check_mark:|:heavy_check_mark:|
## Introduction
This repository contains a Cadmium simulation of a RISC (Reduced Instruction Set Computer) processor. 
The processor is made of 4 pipelined stages and includes instruction, decode, execution and memory read/write stages. 
Each stage runs in parallel to increase the throughput of the system. The processor also utilizes a read only instruction memory and read/write data memory.

## Dependencies
This project assumes that you have Cadmium installed in a location accessible by the environment variable $CADMIUM.
_This dependency would be met by default if you are using the ARSLAB servers. To check, try `echo $CADMIUM` in the terminal_

## Install
To install this project, run:
```sh
git clone https://github.com/ryanrizk1/DEVS-RISC-Processor.git
```

## Build
To build this project, run:
```sh
source build_sim.sh
```
__NOTE__: Everytime you run build_sim.sh, the contents of `build/` and `bin/` will be replaced.

## Execute
To run the project, run:
```sh
./bin/sample_project
```

## Log Results
To log the output of the simulation, run:
```sh
./bin/sameple_project > logs.log
```
