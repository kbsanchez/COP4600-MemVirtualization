#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>

#define PAGE_SIZE 4096 //Page sized is assumed 4KB, 4096 Bytes

struct pt_entry
{
    int PID;
    int VPN;
    int offset;
    int present;
    int dirty;
};

int events_cnt = 0;
int hits_cnt = 0;
int fault_cnt = 0;
int reads_cnt = 0;
int writes_cnt = 0;

int main(int argc, char *argv[]) {
    char* in_file = argv[1];
    int num_frames = atoi(argv[2]);
    std::string alg = argv[3];
    std::string mode;
    int p;
    if (alg == "vms"){
        p = atoi(argv[4]);
        mode = argv[5];
    }
    else if (alg == "lru" || alg == "fifo"){
        mode = argv[4];
    }
    else{
        std::cout << "Error. Invalid algorithm.\n";
        return -1;
    }


    if (num_frames <= 0) {
        std::cout << "Error. Invalid number of frames.\n";
        return -1;
    }

    /* --------------------------------------------
    //Also add mode check somewhere here.
        //Would also be a good idea to add a check if LRU, FIFO have the P parameter when they shouldn't 
        / VMS doesn't have when it should.
    ----------------------------------------------- */
    
    if(alg == "vms"){
        
        
        if((p < 1) || (p > 100)){
            std::cout << "Error. Percentage must be a number between 1 and 100.\n";
            return -1;
        }
        
        //Making sure inputs are being retrieved
        std::cout << "infile: " << in_file << "\n"
            << "num_frames: " << num_frames << "\n"
            << "alg: " << alg << "\n"
            << "p: " << p << "\n"
            << "mode: " << mode << "\n\n";
    }
    else{
        
        //Making sure inputs are being retrieved
        std::cout << "infile: " << in_file << "\n"
            << "num_frames: " << num_frames << "\n"
            << "alg: " << alg << "\n"
            << "mode: " << mode << "\n\n";
    }
    //std::ifstream trace;
    
    
    
    
    FILE *trace = fopen(in_file, "r");
    if (trace == NULL){
        std::cout << "Failed to open requested file.\n";
        return -1;
    }
    
    unsigned addr;
    char rw;

    while(fscanf(trace, "%x %c", &addr, &rw) != EOF){
        ++events_cnt;
        
        pt_entry *newEntry = new pt_entry;
        if(strcmp(&rw, "R")){
            ++reads_cnt; //this will be incremented within the algo during faults i think? but we can keep it here to test
            newEntry->dirty = 0;
        }else if(strcmp(&rw, "W")){
            ++writes_cnt; //<-- this one too
            newEntry->dirty = 1;
        }
    }

    std::cout << "total memory frames: " << num_frames << "\n"
        << "events in trace: " << events_cnt << "\n"
        << "total disk reads: " << reads_cnt << "\n"
        << "writes: " << writes_cnt << "\n";
}