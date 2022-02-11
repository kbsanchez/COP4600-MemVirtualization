#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#define PAGE_SIZE 4096 //Page sized is assumed 1KB, 4096 Bytes


int main(int argc, char *argv[]) {
    std::string in_file = argv[1];
    int num_frames = atoi(argv[2]);
    std::string alg = argv[3];


    if (num_frames <= 0) {
        printf("Error. Invalid number of frames.\n");
        return -1;
    }

    if(alg == "vms"){
        //Taking an extra parameter for vms
        int p = atoi(argv[4]);
        
        if((p < 1) || (p > 100)){
            printf("Error. Percentage must be a number between 1 and 100.\n");
            return -1;
        }

        std::string mode = argv[5];
            //Making sure inputs are being retrieved
    std::cout << "infile: " << in_file << "\n"
              << "num_frames: " << num_frames << "\n"
              << "alg: " << alg << "\n"
              << "p: " << p << "\n"
              << "mode: " << mode << "\n";
    }else{
        std::string mode = argv[4];
            //Making sure inputs are being retrieved
    std::cout << "infile: " << in_file << "\n"
              << "num_frames: " << num_frames << "\n"
              << "alg: " << alg << "\n"
              << "mode: " << mode << "\n";
    }
}