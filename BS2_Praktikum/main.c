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
#include "my_semaphore.h"
#include "p02_time_reader.h"


void time_writer_main(int shared_id,my_sem_id mutex,my_sem_id wri);
struct shared_struct *time_writer_global = NULL;

//Damn signal handler design forces me to use globals
int shared_id = -1;
my_sem_id mutex = -1; my_sem_id wri = -1;
bool runFlag = true;

void cleanup (int i)
{
    runFlag = false;
    printf("Got signal %d\n",i);
    
    if (!time_writer_global) {shmdt(time_writer_global); time_writer_global = NULL;}
    if (shared_id != -1) {
        if (shmctl(shared_id, IPC_RMID, NULL) != 0) {
            perror("Could not destroy shared memory");
        }
        shared_id = -1;
    }
    if (mutex != -1) {delete_sem(mutex); mutex = -1;}
    if (wri != -1) {delete_sem(wri); wri = -1;}
}

void set_My_Timer(unsigned set, unsigned reset)
{
    const struct itimerval itimerval = {{set,0},{reset,0}};
    
    if (setitimer(ITIMER_REAL, &itimerval, NULL) != 0) {
        perror("Cannot set timer");
    }
}

struct shared_struct
{
    uint_fast16_t rc;
    char time_string[9];
};

void time_writer(int i)
{
    if (!runFlag) return;
    assert(i == SIGALRM);
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    P(wri);
    
    sprintf(time_writer_global->time_string,"%d:%d.XX",tm.tm_hour,tm.tm_min);
    
    V(wri);
    set_My_Timer(0, 1);
}


int main(int argc, const char * argv[]) {
    
    if (signal(SIGQUIT, cleanup) == SIG_ERR) {perror("Cound not install a signalhandler"); exit(EXIT_FAILURE);}
    if (signal(SIGTERM,cleanup) == SIG_ERR) {perror("Cound not install a signalhandler"); exit(EXIT_FAILURE);}
    if (signal(SIGINT,cleanup) == SIG_ERR) {perror("Cound not install a signalhandler"); exit(EXIT_FAILURE);}
    uint_fast16_t i;
    const uint_fast16_t child_count = 1; pid_t childs[child_count];
    
    shared_id = shmget(IPC_PRIVATE, sizeof(struct shared_struct), IPC_CREAT|0600);
    if (shared_id == -1) {
        perror("Shared memory creation failed");
        exit(EXIT_FAILURE);
    }
    mutex = create_sem(1); wri = create_sem(1);
    
    struct shared_struct *shared_mem = (struct shared_struct*) shmat(shared_id, NULL, 0);
    
    memset(shared_mem,0,sizeof(*shared_mem));
    
    for(i = 0; i < child_count; i++)
    {
        childs[i] = fork();
        if (childs[i] == 0)
        {
            //Child
            return reader(shared_id,mutex,wri);
        } else if(childs[i] == -1)
        {
            perror("Fork of child failed");
            return EXIT_FAILURE;
        }
        else
        {
            time_writer_main(shared_id,mutex,wri);
        }
    }
    
    for (i = 0; i < child_count; i++) {
        if (kill(childs[i], SIGQUIT) == -1) perror("Cant kill a child");
    }
    cleanup(0);
    return EXIT_SUCCESS;
}

void time_writer_main(int shared_id,my_sem_id mutex,my_sem_id wri)
{
    time_writer_global = (struct shared_struct*) shmat(shared_id, NULL, 0);
    
    if (signal(SIGALRM, time_writer) == SIG_ERR)
    {
        perror("Cannot set alarm handler");
    }
    set_My_Timer(0, 1);
    
    char foo[64];
    do {
        fgets(foo, 64, stdin);
    } while (strchr(foo,'q') == NULL);
}
