# Project 2: Memory Virtualization
#### This project was produced with the intention to read in a memory trace and simulate the actions of a virtual memory system with a single level page table. The three page replacement algorithms implemented for this simulator are:
1. FIFO (First In, First Out)
2. LRU (Least Recently Used)
3. Segmented FIFO
## How to compile:
1. Use the command "make", and it will compile a memsim executable
2. Alternatively, you can compile using "g++ -o memsim memsim.cpp" to compile
## How to run:
1. FIFO and LRU format:
#### ./memsim tracefile nFrames policy quiet/debug
