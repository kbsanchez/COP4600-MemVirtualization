#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#define PAGE_SIZE 4096 //Page sized is assumed 1KB, 4096 Bytes


int main(int argc, char *argv[]) {
    char *in_file = argv[1];
    int num_frames = atoi(argv[2]);
    char *alg = argv[3];
    int p = atoi(argv[4]);
    char *mode = argv[5];

    if (num_frames <= 0) {
        printf("Error. Invalid number of frames.\n");
        return -1;
    }

    //Making sure inputs are being retrieved
    std::cout << "infile: " << in_file << "\n"
              << "num_frames: " << num_frames << "\n"
              << "alg: " << alg << "\n"
              << "p: " << p << "\n"
              << "mode: " << mode << "\n";
}