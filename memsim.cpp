#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

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
std::vector<pt_entry*> secondary_buffer;

int get_time_accessed() {
    return ++elapsed_time;
}

pt_entry* is_present(pt_entry *entry) {
        for (int i = 0; i < page_table.size(); i++) {
            if (page_table[i]->VPN == entry->VPN){
                return page_table[i];
            }
        }

        return NULL;
}

pt_entry* is_present_secondary(pt_entry *entry) {
        for (int i = 0; i < secondary_buffer.size(); i++) {
            if (secondary_buffer[i]->VPN == entry->VPN){
                return secondary_buffer[i];
            }
        }

        return NULL;
}

void fifo(pt_entry *PTE, int nframes){
    pt_entry *locale = is_present(PTE);
    if (locale == NULL) { //if not present
        fault_cnt++;
        reads_cnt++;
        //fault and must read from disk

        if (page_table.size() == nframes){ //if page table is full
            //pop first
            //push
            if (page_table[0]->dirty == 1) { //write to disk is entry to be removed is dirty
                writes_cnt++;
            }
            page_table.erase(page_table.begin()); //erase fifo element
            page_table.push_back(PTE);
        }
        else { //if not full
            page_table.push_back(PTE);
        }
    }
    else { //if present
        hits_cnt++;
        if (PTE->dirty == 1)
        {
            locale->dirty = 1;
        }
    }
}

void lru(pt_entry *PTE, int nframes){
    pt_entry *locale = is_present(PTE);
    
    if(locale == NULL){ //if not present
        fault_cnt++;
        reads_cnt++;

        if(page_table.size() == nframes){   //if page table is full
            int smallest = 1000001;
            int lru = 0;
            for(int i = 0; i < page_table.size(); i++){         //search through table for smallest
                if(page_table[i]->time_accessed < smallest){
                    smallest = page_table[i]->time_accessed;
                    lru = i;
                }
            }

            if (page_table[lru]->dirty == 1) {
                writes_cnt++;
            }

            page_table.erase(page_table.begin() + lru);     //erase lru element
            page_table.push_back(PTE);     
        }
        else{   //if not full
            PTE->time_accessed = elapsed_time;  //update elapsed time
            page_table.push_back(PTE);
        }

    }
    else{   //if present
        hits_cnt++;                                             
        for (int i = 0; i < page_table.size(); i++) {
            if (page_table[i]->VPN == PTE->VPN){
                page_table[i]->time_accessed = elapsed_time;    //update elapsed time
            }
        }
        if (PTE->dirty == 1){
            locale->dirty = 1;
        }
    }
}

void segmented_fifo(pt_entry *PTE, int nframes, int p){
    int secondaryFrames = nframes * (p * .01);
    int primaryFrames = nframes - secondaryFrames;

    if(primaryFrames == 0){ // if p = 100, it emulates lru
        lru(PTE, secondaryFrames);
    }
    else{
        if(secondaryFrames == 0){   // if p = 0, it emulates fifo
            fifo(PTE, nframes);
        }
        else{
            //CHECK PRESENCE IN BOTH BUFFERS FIRST
            pt_entry* locale_prim = is_present(PTE);
            pt_entry* locale_sec = is_present_secondary(PTE); 
            if (locale_prim == NULL && locale_sec == NULL){     //If absent from both
                fault_cnt++;
                reads_cnt++;
                if (page_table.size() == primaryFrames){    //If prim buffer is full
                    if (secondary_buffer.size() == secondaryFrames){    //If secondary buff is full
                        int smallest = 1000001;                            
                        int lru = 0; 
                        for (int i = 0; i < secondary_buffer.size(); i++){      //find lru entry and evict
                            if(secondary_buffer[i]->time_accessed < smallest){
                                smallest = secondary_buffer[i]->time_accessed;
                                lru = i;
                            }
                        }
                        if (secondary_buffer[lru]->dirty == 1){
                            writes_cnt++;       //increment write count
                        }
                        secondary_buffer.erase(secondary_buffer.begin() + lru);
                        for (int i = 0; i < secondary_buffer.size(); i++)
                        {
                            secondary_buffer[i]->time_accessed = elapsed_time;      //update elapsed time sec buffer, in slides this is done before val is inserted
                        }
                        pt_entry* oldestInPrimary = page_table[0];
                        oldestInPrimary->time_accessed = 1;     //resetting access time when value is pushed to sec buffer, just in case this val was in sec buffer before already
                        secondary_buffer.push_back(oldestInPrimary);    //push oldest in primary to sec buffer
                        page_table.push_back(PTE);      //push new entry to primary 
                        page_table.erase(page_table.begin());       //remove oldest from primary
                    }
                    else {      //secondary buff is not full
                        page_table.push_back(PTE);      //add to prim buffer
                        for (int i = 0; i < secondary_buffer.size(); i++)
                        {
                            secondary_buffer[i]->time_accessed = elapsed_time;      //update sec buffer access times
                        }
                        pt_entry* oldestInPrimary = page_table[0];
                        oldestInPrimary->time_accessed = 1;     //reseting access time
                        secondary_buffer.push_back(oldestInPrimary);    //add fifo val to sec buffer
                        page_table.erase(page_table.begin());       //remove fifo element from prim buffer
                    }
                }
                else {      //prim buff is not full
                    page_table.push_back(PTE);
                }
            }
            else if (locale_prim != NULL) {     //present in primary buffer
                hits_cnt++;
                if (PTE->dirty == 1){
                    locale_prim->dirty = 1;
                }
            }
            else if (locale_sec != NULL) {      //present in secondary buffer
                int index = 0;
                hits_cnt++;
                if (locale_sec->dirty == 1)
                {
                    PTE->dirty = 1;
                }
                page_table.push_back(PTE);      //pushing hit to prim buffer
                for (int i = 0; i < secondary_buffer.size(); i++)
                {
                    if ((secondary_buffer[i]->VPN) == (locale_sec->VPN))
                    {
                        index = i;
                    }
                }
                secondary_buffer.erase(secondary_buffer.begin() + index);       //removing hit from secondary buffer
                for (int i = 0; i < secondary_buffer.size(); i++)
                {
                    secondary_buffer[i]->time_accessed = elapsed_time;      //updating access times
                }
                pt_entry* oldestInPrimary = page_table[0];
                oldestInPrimary->time_accessed = 1;         //reset access time of entry to be added to sec buffer
                secondary_buffer.push_back(oldestInPrimary);        //adding oldest in primary to sec buffer
                page_table.erase(page_table.begin());       //removing oldest in primary from primary buffer
            }
            else{       //This shouldnt happen, the new entry cannot be in both buffers
                std::cout << "If you're reading this, it's too late.\n";
            }
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
        if ((p < 0) || (p > 100)){
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
    if (mode != "debug" && mode!= "quiet"){
        std::cout << "Error. Mode must be debug or quiet.\n";
            return -1;
    }
    
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
        if(rw == 'R'){
            newEntry->dirty = 0;
        }else if(rw == 'W'){
            newEntry->dirty = 1;
        }                

        newEntry->VPN = addr / PAGE_SIZE;
        newEntry->time_accessed = get_time_accessed();

        if (alg == "fifo"){
            //Send entry to fifo
            fifo(newEntry, num_frames);
        }
        else if (alg == "lru"){
            //Send entry to lru
            lru(newEntry, num_frames);
        }
        else if (alg == "vms"){
            //Send entry to vms
            segmented_fifo(newEntry, num_frames, p);
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