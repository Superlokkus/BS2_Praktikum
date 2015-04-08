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
#include <sys/shm.h>
#include "my_semaphore.h"

int reader(int shared_id,my_sem_id mutex,my_sem_id wri);
int writer(int shared_id,my_sem_id mutex,my_sem_id wri);

struct shared_struct
{
    uint_fast16_t rc;
};

int main(int argc, const char * argv[]) {
    uint_fast16_t i;
    const uint_fast8_t child_count = 20; pid_t childs[child_count];
    int shared_id = shmget(IPC_PRIVATE, sizeof(struct shared_struct), IPC_CREAT|0600);
    if (shared_id == -1) {
        perror("Shared memory creation failed");
        exit(EXIT_FAILURE);
    }
    my_sem_id mutex = create_sem(1); my_sem_id wri = create_sem(1);
    
    struct shared_struct *shared_mem = (struct shared_struct*) shmat(shared_id, NULL, 0);
    
    shared_mem->rc = 0;
    
    for(i = 0; i < child_count; i++)
    {
        childs[i] = fork();
        if (childs[i] == 0)
        {
            //Child
            if (!(i % 5)) {
                return writer(shared_id,mutex,wri);
            } else
            {
                return reader(shared_id,mutex,wri);
            }
        } else if(childs[i] == -1)
        {
            perror("Fork of child failed");
            return EXIT_FAILURE;
        }
    }
    
    for (i = 0; i < child_count;) {
        int child_return_value;
        pid_t wait_return = waitpid(childs[i], &child_return_value,0);
        if (wait_return == -1)
        {
            if (errno == EINTR) {
                perror("Waitpid unblocked early because");
                continue;
            } else
            {
                perror("Waitpid failed");
                exit(EXIT_FAILURE);
            }
        }
        if (WIFEXITED(child_return_value)) {
            printf("Child Nr. %d exited with %d\n",i+1,WEXITSTATUS(child_return_value));
            //while( (wpid = wait(&exitStatus)) > 0)
        }
        i++;
    }
    delete_sem(mutex); delete_sem(wri);
    if (shmctl(shared_id, IPC_RMID, NULL) != 0) {
        perror("Could not destroy shared memory");
    }
    return EXIT_SUCCESS;
}

int reader(int shared_id,my_sem_id mutex,my_sem_id wri)
{
    struct shared_struct *shared_mem = (struct shared_struct*) shmat(shared_id, NULL, 0);
    P(mutex);
    shared_mem->rc++;
    if (shared_mem->rc == 1) {
        P(wri);
    }
    V(mutex);
    printf("I'm reading stuff!\n");
    P(mutex);
    shared_mem->rc--;
    if (shared_mem->rc == 0) {
        V(wri);
    }
    V(mutex);
    return EXIT_SUCCESS;
}

int writer(int shared_id,my_sem_id mutex,my_sem_id wri)
{
    printf("I'm going to write stuff!\n");
    P(wri);
    printf("I'm writing stuff!\n");
    V(wri);
    printf("I wrote stuff!\n");
    
    return EXIT_SUCCESS;
}
