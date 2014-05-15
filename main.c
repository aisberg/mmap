//
//  main.c
//  mmap
//
//  Created by Макс on 16.05.14.
//  Copyright (c) 2014 Максим Никитин. All rights reserved.
//

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timeb.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <limits.h>

struct timeval this_time, this_time_end, res;

const char * mem_name = "special_name_for_shm_memory";


int memd;

int first_mmap(int size) {
    char * addr_first = mmap(NULL, size, PROT_WRITE, MAP_SHARED, memd, 0);
    if (addr_first == MAP_FAILED) {
        return -1;
    }
    
    int i;
    for (i = 0; i < size; ++i) {
        addr_first[i] = i;
    }
    
    if (munmap(addr_first, size)) {
        return -1;
    }
    
    return 0;
}

void second_mmap(int size) {
    char * addr_second = mmap(NULL, size, PROT_READ, MAP_SHARED, memd, 0);
    if (addr_second == MAP_FAILED) {
        perror("mmap second");
        exit(0);
    }
    
    int i;
    for (i = 0; i < size; ++i) {
        printf("\\%d/", addr_second[i]);
    }
    
    printf("\n");
    
    if (munmap(addr_second, size)) {
        perror("munmap second");
        exit(0);
    }
}



int main(int argc, char *argv[]) {
    
    if(argc != 2){
        perror("mmm... sir, it's not good!");
        exit(0);
    }
    
    int size = atoi(argv[1]);
    
    gettimeofday(&this_time, 0);
    
    memd = shm_open(mem_name, O_RDWR | O_CREAT, 0666);
    if (memd == -1) {
        perror("shm open");
        exit(0);
    }
    
    if (ftruncate(memd, size)) {
        perror("ftruncate");
        exit(0);
    }
    
    int for_first;
    if ((for_first = first_mmap(size))) {
        perror("first_mmap");
    }
    
    
    
    if (fork()){
        wait(NULL);
    }
    else{
        second_mmap(size);
        exit(0);
    }
    
    
    
    if (shm_unlink(mem_name)) {
        perror("shm unlink");
        exit(0);
    }
    
    gettimeofday(&this_time_end, 0);
    res.tv_sec= this_time_end.tv_sec -this_time.tv_sec;
    res.tv_usec=this_time_end.tv_usec-this_time.tv_usec;
    if(res.tv_usec < 0) {
        res.tv_sec--;
        res.tv_usec+=1000000;
    }
    printf("%ld,%u sec \n", res.tv_sec, res.tv_usec);
    
    return 0;
}


