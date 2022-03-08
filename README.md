# Project 2: Memory Virtualization
#### This project was produced with the intention to read in a memory trace and simulate the actions of a virtual memory system with a single level page table. The three page replacement algorithms implemented for this simulator are:
1. FIFO (First In, First Out)
2. LRU (Least Recently Used)
3. Segmented FIFO
#### 
#### Make sure that your tracefile is in the same directory as this program.
## How to compile:
1. Use the command "make", and it will compile a memsim executable
2. Alternatively, you can compile using "g++ -o memsim memsim.cpp" to compile
## How to run:
1. FIFO and LRU format: ./memsim tracefile nFrames policy quiet/debug
2. VMS format: ./memsim tracefile nFrames policy percentage quiet/debug
##### where: 
- policy: fifo, lru or vms 
- percentage: 1-100 
- tracefile: name of the tracefile 
- nFrames: number of frames to use 
- quiet/debug: quiet will just print the stats while debug will print every event that happens 
## Running examples
##### ./memsim bzip.trace 32 fifo quiet
##### total memory frames: 32
##### events in trace: 1000000
##### total disk reads: 2497
##### total disk writes: 851
##### total faults: 2497
#
##### ./memsim bzip.trace 16 lru quiet 
##### total memory frames: 16
##### events in trace: 1000000
##### total disk reads: 3344
##### total disk writes: 1069
##### total faults: 3344
##### total hits: 996656
#
##### ./memsim bzip.trace 64 vms 50 quiet
##### total memory frames: 64
##### events in trace: 1000000
##### total disk reads: 1290
##### total disk writes: 427
##### total faults: 1290
##### total hits: 998710
