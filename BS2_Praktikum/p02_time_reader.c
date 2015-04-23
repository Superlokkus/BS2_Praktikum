//
//  p02_time_reader.c
//  BS2_Praktikum
//
//  Created by Markus Klemm on 15.04.15.
//  Copyright (c) 2015 net.markusklemm. All rights reserved.
//

#include "p02_time_reader.h"

struct shared_struct
{
    uint_fast16_t rc;
    char time_string[9];
};

int reader(int shared_id,my_sem_id mutex,my_sem_id wri)
{
    struct shared_struct *shared_mem = (struct shared_struct*) shmat(shared_id, NULL, 0600);
    while (runFlag) {
        P(mutex);
        shared_mem->rc++;
        if (shared_mem->rc == 1) {
            P(wri);
        }
        V(mutex);
        printf("I read %s\n",shared_mem->time_string);
        P(mutex);
        shared_mem->rc--;
        if (shared_mem->rc == 0) {
            V(wri);
        }
        V(mutex);
    }
    shmdt(shared_mem);
    return EXIT_SUCCESS;
}