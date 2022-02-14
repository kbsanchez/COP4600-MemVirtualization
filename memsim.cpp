#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>

#define PAGE_SIZE 4096 //Page sized is assumed 4KB, 4096 Bytes

int events_cnt = 0;
int hits_cnt = 0;
int fault_cnt = 0;
int reads_cnt = 0;
int writes_cnt = 0;

int main(int argc, char *argv[]) {
    char* in_file = argv[1];
    int num_frames = atoi(argv[2]);
    std::string alg = argv[3];


    if (num_frames <= 0) {
        std::cout << "Error. Invalid number of frames.\n";
        return -1;
    }

    if(alg == "vms"){
        //Taking an extra parameter for vms
        int p = atoi(argv[4]);
        
        if((p < 1) || (p > 100)){
            std::cout << "Error. Percentage must be a number between 1 and 100.\n";
            return -1;
        }

        std::string mode = argv[5];
        //Making sure inputs are being retrieved
        std::cout << "infile: " << in_file << "\n"
            << "num_frames: " << num_frames << "\n"
            << "alg: " << alg << "\n"
            << "p: " << p << "\n"
            << "mode: " << mode << "\n";
    }
    else{
        
        std::string mode = argv[4];
        //Making sure inputs are being retrieved
        std::cout << "infile: " << in_file << "\n"
            << "num_frames: " << num_frames << "\n"
            << "alg: " << alg << "\n"
            << "mode: " << mode << "\n";
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
        if(strcmp(&rw, "R")){
            ++reads_cnt;
        }else if(strcmp(&rw, "W")){
            ++writes_cnt;
        }
        std::cout << "events: " << events_cnt
            << "\treads: " << reads_cnt << "\twrites: " << writes_cnt << "\n";
    }
}