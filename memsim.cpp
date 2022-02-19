#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#define PAGE_SIZE 4096 //Page sized is assumed 4KB, 4096 Bytes

struct pt_entry
{
    int PID;
    int dirty;
    int time_accessed; //need this for LRU
    unsigned VPN;
};

int events_cnt = 0;
int hits_cnt = 0;
int fault_cnt = 0;
int reads_cnt = 0;
int writes_cnt = 0;
int elapsed_time = 0;

std::vector<pt_entry*> page_table;



int get_time_accessed() {
    return ++elapsed_time;
}



pt_entry* is_present(pt_entry *entry) {
    for (int i = 0; i < page_table.size(); i++) {
        if (page_table[i]->VPN == entry->VPN){
            return page_table[i];
        }
        else{
            return nullptr;
        }
    }
}



void fifo(pt_entry *PTE, int nframes){
    pt_entry *locale = is_present(PTE);
    if (locale == nullptr) {
        fault_cnt++;
        reads_cnt++;
        //page not found, fault and must read from disk

        if (page_table.size() == nframes){
            //table is full
            //pop first
            //push
            if (page_table[0]->dirty == 1) {
                writes_cnt++;
            }
            page_table.erase(page_table.begin());
            page_table.push_back(PTE);
        }
        else {
            page_table.push_back(PTE);
        }
    }
    else {
        hits_cnt++;
        if (PTE->dirty == 1)
        {
            locale->dirty = 1;
        }
    }
}



int main(int argc, char *argv[]) {
    char* in_file = argv[1];
    int num_frames = atoi(argv[2]);
    std::string alg = argv[3];
    std::string mode = "";
    int p;

    //Algorithm validation
    if (alg == "vms"){
        p = atoi(argv[4]);

        //Percentage validation
        if ((p < 1) || (p > 100)){
            std::cout << "Error. Percentage must be a number between 1 and 100.\n";
            return -1;
        }

        mode = argv[5];
    }
    else if (alg == "lru" || alg == "fifo"){
        if(!strcmp(argv[4], "debug\n") || !strcmp(argv[4], "quiet\n")){
            std::cout << "Error. Percentage parameter only needed for vms algorithm.\n";
            return -1;
        }
        else{
            mode = argv[4];
        }
    }
    else{
        std::cout << "Error. Invalid algorithm.\n";
        return -1;
    }

    //Number of frames validation
    if (num_frames <= 0) {
        std::cout << "Error. Invalid number of frames.\n";
        return -1;
    }

    //Mode validation
    if (mode == "debug"){
        //do something
    }
    else if (mode == "quiet"){
        //do something
    }
    else{
        std::cout << "Error. Mode must be debug or quiet.\n";
            return -1;
    }
    
    //
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
            //++reads_cnt; //this will be incremented within the algo during faults i think? but we can keep it here to test
            newEntry->dirty = 0;
        }else if(strcmp(&rw, "W")){
            //++writes_cnt; //<-- this one too
            newEntry->dirty = 1;
        }                 //sounds good!

        newEntry->VPN = addr / PAGE_SIZE;
        newEntry->time_accessed = get_time_accessed();

        if (alg == "fifo"){
            //Send entry to fifo
            fifo(newEntry, num_frames);
        }
        else if (alg == "lru"){
            //Send entry to lru
        }
        else if (alg == "vms"){
            //Send entry to vms
        }
        else{
            std::cout << "Error\n";
            return -1;
        }
    }


    std::cout << "total memory frames: " << num_frames << "\n"
        << "events in trace: " << events_cnt << "\n"
        << "total disk reads: " << reads_cnt << "\n"
        << "total disk writes: " << writes_cnt << "\n"
        << "total faults: " << fault_cnt << "\n"
        << "total hits: " << hits_cnt << "\n";
}