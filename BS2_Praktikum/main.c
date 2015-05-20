//
//  main.c
//  BS2_Praktikum
//
//  Created by Markus Klemm on 08.04.15.
//  Copyright (c) 2015 net.markusklemm. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "my_semaphore.h"


int main(int argc, const char * argv[]) {
    
    if (argc != 3)
    {
        fprintf(stderr, "Usage %s <SourceFile> <DestinationFile>\n",argv[0]);
        return EXIT_FAILURE;
    }

    int source_file_desc = open(argv[1], O_RDONLY);
    if (source_file_desc == -1) {
        perror("Can't open source file");
        return EXIT_FAILURE;
    }
    struct stat source_info;
    if (stat(argv[1], &source_info) != 0) {
        perror("Can't get source file infos");
        return EXIT_FAILURE;
    }
    void *source_mem = mmap(NULL, source_info.st_size, PROT_READ, MAP_FILE|MAP_PRIVATE, source_file_desc, 0);
    if (source_mem == MAP_FAILED) {
        perror("Mapping source file failed");
        return EXIT_FAILURE;
    }
    
    int destination_file_desc = open(argv[2], O_TRUNC|O_CREAT|O_RDWR);
    if (destination_file_desc == -1) {
        perror("Can't open destination file");
    }
    if (chmod(argv[2], source_info.st_mode) != 0) {
        perror("Can't copy file permissions");
    }
    if (lseek(destination_file_desc, source_info.st_size-1, SEEK_SET) == -1) {
        perror("Can'T seek to new end of destination file");
    }
    unsigned char dummy = 0;
    if (write(destination_file_desc,  &dummy, 1) == -1)
    {
        perror("Couldn't write dummy byte");
    }
    
    
    void *destination_mem = mmap(NULL, source_info.st_size, PROT_WRITE,MAP_FILE|MAP_SHARED, destination_file_desc,0);
    if (destination_mem == MAP_FAILED) {
        perror("Mapping destination file failed");
    }
    
    memcpy(destination_mem, source_mem, source_info.st_size);
    
    munmap(source_mem,source_info.st_size);
    munmap(destination_mem, source_info.st_size);
    close(source_file_desc);
    close(destination_file_desc);
    
    return EXIT_SUCCESS;
}



